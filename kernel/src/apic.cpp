#include "apic.hpp"
#include "idt_handler.hpp"
#include "io.h"
#include "nekos/kprintf.hpp"
#include <stdint.h>

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

APIC::APIC(Tag *TagList) {
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
  discoverMADT(TagList);
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

void APIC::discoverMADT(Tag *TagList) {}

APIC &APIC::getAPIC(Tag *TagList) {
  static APIC Apic(TagList);
  return Apic;
}

volatile uint32_t *APIC::getLAPICAddress() { return LAPICAddress; }
void APIC::emitEOI() { LAPICAddress[0xB0 / 4] = 0; }
uint32_t APIC::getLAPICError() { return LAPICAddress[0x280 / 4]; }

InterruptHandler &APIC::IH = InterruptHandler::getInterruptHandler();
volatile uint32_t *APIC::LAPICAddress = nullptr;
