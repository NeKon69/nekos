#pragma once

#include "idt_handler.hpp"
#include "nekos/kernel.hpp"
#include <stdint.h>

// APIC stands for Advanced Programmable Interrupt Controller
class APIC {
public:
  // You are required to provide the tag list on the first call.
  static APIC &getAPIC(MB2Tag *TagList = nullptr);

private:
  friend void LVTTimerHandler(const Registers *Regs);
  friend void LVTErrorHandler(const Registers *Regs);

  APIC(MB2Tag *TagList);

  static void emitEOI();

  // LAPIC stands for Local APIC
  void discoverLAPIC();
  // SIV stands for Spurious Interrupt Vector.
  void setupSIV();
  void setupIOAPIC(MB2Tag *TagList);
  void readAPICID();
  void readAPICMetadata();
  // LVT stands for Local Vector Table.
  void setupLVTEntries();

  static volatile uint32_t *LAPICAddress;
  static InterruptHandler &IH;
};
