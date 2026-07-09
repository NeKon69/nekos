#include "nekos/interrupts/madt.hpp"

#include "attributes.h"
#include "nekos/assert.hpp"

namespace nekos::interrupts {
namespace {
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
}

uint32_t *discoverMADT(MB2Tag *TagList) {
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
    nekosAssert(validateRSDP(Data, Size));
    UsingXSDT = Size >= sizeof(RSDP2Descriptor) &&
                reinterpret_cast<RSDPDescriptor *>(Data)->Revision >= 2 &&
                reinterpret_cast<RSDP2Descriptor *>(Data)->XSDTAddress != 0;
    Header = reinterpret_cast<ACPISDTHeader *>(
        UsingXSDT ? reinterpret_cast<RSDP2Descriptor *>(Data)->XSDTAddress
                  : reinterpret_cast<RSDPDescriptor *>(Data)->RsdtAddress);
    nekosAssert(
        validateAPICSDT(reinterpret_cast<char *>(Header), Header->Length));
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
  nekosAssert(Header != nullptr);
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
  nekosAssert(MADTPhys != 0);
  return reinterpret_cast<uint32_t *>(MADTPhys);
}

MADTIOAPIC *findIOAPIC(uint32_t *MADT) {
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
  MADTIOAPIC *IOAPIC = nullptr;
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
  nekosAssert(IOAPIC != nullptr);
  return IOAPIC;
}
}
