#pragma once

#include "idt_handler.hpp"
#include "nekos/kernel.hpp"
#include <stdint.h>

// APIC stands for Advanced Programmable Interrupt Controller
class APIC {
public:
  // You are required to provide the tag list on the first call.
  static APIC &getAPIC(MB2Tag *TagList = nullptr);
  static volatile uint32_t *getLAPICAddress();
  static void emitEOI();
  static uint32_t getLAPICError();

private:
  APIC(MB2Tag *TagList);

  // LAPIC stands for Local APIC
  void discoverLAPIC();
  // MADT stands for Multiple APIC Description Table
  uint32_t *discoverMADT(MB2Tag *TagList);
  struct MADTRecord {
    uint8_t Type;
    uint8_t Length;
  } __attribute__((packed));
  struct MADTIOAPIC : MADTRecord {
    uint8_t ID;
    uint8_t Reserved;
    uint32_t Address;
    uint32_t GlobalBaseIRQ;
  } __attribute__((packed));
  void discoverIOAPIC(uint32_t *MADTAddress);
  // SIV stands for Spurious Interrupt Vector.
  void setupSIV();
  void setupIOAPIC(MB2Tag *TagList);
  void readAPICID();
  void readAPICMetadata();
  // LVT stands for Local Vector Table.
  void setupLVTEntries();

  static volatile uint32_t *LAPICAddress;
  static InterruptHandler &IH;
  static MADTIOAPIC *IOAPIC;
};
