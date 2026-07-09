#include "apic.hpp"
#include "attributes.h"
#include "idt_handler.hpp"
#include "io.h"
#include "nekos/kprintf.hpp"
#include <stdint.h>
#include <string.h>

// Values from
// https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/07_APIC.md#disabling-the-pic8259
constexpr uint16_t PICCommandMaster = 0x20;
constexpr uint16_t PICCommandSlave = 0xA0;
constexpr uint16_t PICDataMaster = PICCommandMaster + 1;
constexpr uint16_t PICDataSlave = PICCommandSlave + 1;
constexpr uint8_t PICInitCommand = 0x11;
// Start at 0x20 since 0-31 are reserved
constexpr uint8_t PICMasterVector = 0x20;
// Skip 8 vectors (handled by master)
constexpr uint8_t PICSlaveVector = PICMasterVector + 8;
// A way for slave/master to communicate
constexpr uint8_t PICSlavePin = 2;
constexpr uint8_t PICMasterPin = 1 << PICSlavePin;
constexpr uint8_t PIC8086Mode = 0x01;
// Mask all interrupts
constexpr uint8_t PICMaskInterrupts = 0xFF;

APIC::APIC(MB2Tag *TagList) {
  // PIC expects commands in this specific order, otherwise it won't work.
  outB(PICCommandMaster, PICInitCommand);
  outB(PICCommandSlave, PICInitCommand);
  outB(PICDataMaster, PICMasterVector);
  outB(PICDataSlave, PICSlaveVector);
  outB(PICDataMaster, PICMasterPin);
  outB(PICDataSlave, PICSlavePin);
  outB(PICDataMaster, PIC8086Mode);
  outB(PICDataSlave, PIC8086Mode);
  outB(PICDataMaster, PICMaskInterrupts);
  outB(PICDataSlave, PICMaskInterrupts);
  // Find LAPIC address and set it in the member variable.
  discoverLAPIC();
  setupSIV();
  readAPICID();
  readAPICMetadata();
  setupLVTEntries();
  setupIOAPIC(TagList);
}

void APIC::discoverLAPIC() {
  uint32_t LAPICLow, LAPICHigh [[maybe_unused]];
  // we want to find a local APIC in MSR called IA32_APIC_BASE and its value is
  // 0x1B
  asm volatile("rdmsr" : "=a"(LAPICLow), "=d"(LAPICHigh) : "c"(0x1B));
  kprintf("LAPICLow: %x\n", LAPICLow);
  // First 12 bits are flags and whatever else. we only care about the LAPIC
  // address. (last 20 bits)
  // https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/07_APIC.md#discovering-the-local-apic
  LAPICAddress = reinterpret_cast<uint32_t *>(LAPICLow & 0xFFFFF000);
}

void spuriousInterruptHandler(const Registers *Regs [[maybe_unused]]) {
  kprintf("Spurious interrupt. We are cooked.\n");
  InterruptHandler::halt();
}

void APIC::setupSIV() {
  // Enable Spurious Interrupt Vector (i chose 239th vector as the handler for
  // funs and giggles)
  constexpr uint8_t SpuriousInterruptVector = 0xEF;
  LAPICAddress[0xF0 / 4] = SpuriousInterruptVector | (1 << 8);
  IH.setHandler(SpuriousInterruptVector, spuriousInterruptHandler);
}

void APIC::readAPICID() {
  uint8_t APICID = LAPICAddress[0x20 / 4] >> 24;
  kprintf("APICID: %u\n", APICID);
}

void APIC::readAPICMetadata() {
  // Last 6 bits are reserved, ignore.
  uint32_t APICMetadata = LAPICAddress[0x30 / 4] & 0x01FFFFFF;
  kprintf("APIC Version: %x\n", APICMetadata & 0xFF);
  kprintf("Max LVT Entries: %x\n", (APICMetadata >> 16) & 0xFF);
  kprintf("Suppress EOI Broadcast: %x\n", (APICMetadata >> 24) & 0x1);
}

// https://github.com/dreamportdev/Osdev-Notes/blob/master/02_Architecture/07_APIC.md#local-vector-table

// Heard that 0x0 is the most common and others are only needed in some advanced
// cases, which is certainly not my case, prolly fine.
constexpr uint16_t LVTDeliveryMode = 0x0 << 8;
// LVT always uses physical mode.
constexpr uint16_t LVTDestinationMode = 0b0 << 11;
constexpr uint16_t LVTPinPolarity(bool Low) { return Low << 13; }
constexpr uint16_t LVTTriggerMode(bool LevelTriggered) {
  return LevelTriggered << 15;
}
constexpr uint32_t LVTEntryEnabled(bool Enabled) { return (!Enabled) << 16; }
constexpr uint8_t LVTErrorVector = 32;
constexpr uint8_t LVTTimerVector = 33;

void LVTTimerHandler(const Registers *Regs [[maybe_unused]]) {
  kprintf("Timer interrupt\n");
  APIC::emitEOI();
}

void LVTErrorHandler(const Registers *Regs [[maybe_unused]]) {
  uint32_t error = APIC::getLAPICError();
  kprintf("APIC Error: 0x%x\n", error);
  APIC::emitEOI();
}

void APIC::setupLVTEntries() {
  LAPICAddress[0x320 / 4] = LVTTimerVector | LVTDeliveryMode |
                            LVTDestinationMode | LVTPinPolarity(0) |
                            LVTTriggerMode(0) | LVTEntryEnabled(1);
  LAPICAddress[0x370 / 4] = LVTErrorVector | LVTDeliveryMode |
                            LVTDestinationMode | LVTPinPolarity(0) |
                            LVTTriggerMode(0) | LVTEntryEnabled(1);
  IH.setHandler(LVTTimerVector, LVTTimerHandler);
  IH.setHandler(LVTErrorVector, LVTErrorHandler);
}

void APIC::setupIOAPIC(MB2Tag *TagList) {
  discoverIOAPIC(discoverMADT(TagList));
}

struct ACPISDTHeader {
  char Signature[4];
  uint32_t Length;
  uint8_t Revision;
  uint8_t Checksum;
  char OEMID[6];
  char OEMTableID[8];
  uint32_t OEMRevision;
  uint32_t CreatorID;
  uint32_t CreatorRevision;
} __attribute__((packed));

MB2Tag *next(MB2Tag *Tag NEKOS_LIFETIMEBOUND) {
  // Align the tag by 8 bytes (i hate this).
  uint32_t AlignedSize = Tag->Size;
  if (AlignedSize & 0b111)
    AlignedSize = (AlignedSize & ~0b111) + 8;
  return reinterpret_cast<MB2Tag *>(reinterpret_cast<char *>(Tag) +
                                    AlignedSize);
}
bool validateRSDP(char *ByteArray, uint32_t Size) {
  for (uint8_t I = 0; I < 8; I++)
    if (ByteArray[I] != "RSD PTR "[I])
      return false;
  uint32_t Checksum = 0;
  for (uint32_t I = 0; I < Size; I++)
    Checksum += ByteArray[I];
  // Last byte is supposed to be 0
  return (Checksum & 0xFF) == 0;
}
bool validateAPICSDT(char *ByteArray, uint32_t Size) {
  for (uint8_t I = 0; I < 4; I++)
    if (ByteArray[I] != "RSDT"[I] && ByteArray[I] != "XSDT"[I])
      return false;
  uint32_t Checksum = 0;
  for (uint32_t I = 0; I < Size; I++)
    Checksum += ByteArray[I];
  return (Checksum & 0xFF) == 0;
}

uint32_t *APIC::discoverMADT(MB2Tag *TagList) {
  struct RSDPDescriptor {
    char Signature[8];
    uint8_t Checksum;
    char OEMID[6];
    uint8_t Revision;
    uint32_t RsdtAddress;
  } __attribute__((packed));
  struct RSDP2Descriptor {
    RSDPDescriptor RSDP;
    uint32_t Length;
    uint64_t XSDTAddress;
    uint8_t ExtendedChecksum;
    uint8_t Reserved[3];
  } __attribute__((packed));

  struct RSDT : ACPISDTHeader {
    uint32_t SDTAddresses[];
  } __attribute__((packed));
  struct XSDT : ACPISDTHeader {
    uint64_t SDTAddresses[];
  } __attribute__((packed));

  ACPISDTHeader *Header = nullptr;
  bool UsingXSDT = false;
  auto parseRSDP = [&](char *Data, uint32_t Size) {
    if (!validateRSDP(Data, Size))
      asm volatile("int $254");
    UsingXSDT = Size >= sizeof(RSDP2Descriptor) &&
                reinterpret_cast<RSDPDescriptor *>(Data)->Revision >= 2 &&
                reinterpret_cast<RSDP2Descriptor *>(Data)->XSDTAddress != 0;
    Header = reinterpret_cast<ACPISDTHeader *>(
        UsingXSDT ? reinterpret_cast<RSDP2Descriptor *>(Data)->XSDTAddress
                  : reinterpret_cast<RSDPDescriptor *>(Data)->RsdtAddress);
    if (!validateAPICSDT(reinterpret_cast<char *>(Header), Header->Length))
      asm volatile("int $254");
  };
  for (MB2Tag *Tag = TagList; Tag != nullptr; Tag = next(Tag)) {
    bool Exit = false;
    switch (Tag->Type) {
    case 0xF:
      parseRSDP(reinterpret_cast<char *>(Tag) + sizeof(MB2Tag),
                sizeof(RSDP2Descriptor));
      break;
    case 0xE:
      parseRSDP(reinterpret_cast<char *>(Tag) + sizeof(MB2Tag),
                sizeof(RSDPDescriptor));
      break;
    case 0x0:
      Exit = true;
      break;
    default:
      break;
    }
    if (Exit)
      break;
  }
  uint32_t Size =
      (Header->Length - sizeof(ACPISDTHeader)) / (UsingXSDT ? 8 : 4);
  uintptr_t MADTPhys = 0;
  for (uint32_t I = 0; I < Size; I++) {
    uintptr_t EntryAddr =
        UsingXSDT ? reinterpret_cast<XSDT *>(Header)->SDTAddresses[I]
                  : reinterpret_cast<RSDT *>(Header)->SDTAddresses[I];
    auto *EntryHdr = reinterpret_cast<ACPISDTHeader *>(EntryAddr);
    bool Found = true;
    for (int J = 0; J < 4; J++)
      if (EntryHdr->Signature[J] != "APIC"[J]) {
        Found = false;
        break;
      }
    if (Found) {
      MADTPhys = EntryAddr;
      break;
    }
  }
  if (!MADTPhys)
    asm volatile("int $254");
  return reinterpret_cast<uint32_t *>(MADTPhys);
}

void APIC::discoverIOAPIC(uint32_t *MADT) {
  auto *Header = reinterpret_cast<ACPISDTHeader *>(MADT);
  // Skip over the 2 additional fields that are added specifically for MADT and
  // we do not care about them:
  //  typedef struct {
  //   EFI_ACPI_DESCRIPTION_HEADER    Header;            // 36 bytes
  //   UINT32                         LocalApicAddress;  // offset 36, 4 bytes
  //   UINT32                         Flags;             // offset 40, 4 bytes
  //  } EFI_ACPI_6_0_MULTIPLE_APIC_DESCRIPTION_TABLE_HEADER;
  // Ignore LocalApicAddress and Flags to get to the actual MADT entries.
  auto *Ptr = reinterpret_cast<uint8_t *>(MADT) + sizeof(ACPISDTHeader) + 8;
  auto *End = reinterpret_cast<uint8_t *>(MADT) + Header->Length;
  IOAPIC = nullptr;
  while (Ptr < End) {
    auto *Record = reinterpret_cast<MADTRecord *>(Ptr);
    switch (Record->Type) {
    case 1:
      IOAPIC = reinterpret_cast<MADTIOAPIC *>(Ptr);
      break;
    default:
      break;
    }
    Ptr += Record->Length;
  }
  if (!IOAPIC)
    asm volatile("int $254");
}
APIC &APIC::getAPIC(MB2Tag *TagList) {
  static APIC Apic(TagList);
  return Apic;
}

volatile uint32_t *APIC::getLAPICAddress() { return LAPICAddress; }
void APIC::emitEOI() { LAPICAddress[0xB0 / 4] = 0; }
uint32_t APIC::getLAPICError() { return LAPICAddress[0x280 / 4]; }

InterruptHandler &APIC::IH = InterruptHandler::getInterruptHandler();
volatile uint32_t *APIC::LAPICAddress = nullptr;
APIC::MADTIOAPIC *APIC::IOAPIC = nullptr;
