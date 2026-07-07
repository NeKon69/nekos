#pragma once

#include "idt_handler.hpp"
#include "nekos/kernel.hpp"
#include <stdint.h>

// APIC stands for Advanced Programmable Interrupt Controller
class APIC {
public:
  // You are required to provide the tag list on the first call.
  static APIC &getAPIC(Tag *TagList = nullptr);
  static volatile uint32_t *getLAPICAddress();
  static void emitEOI();
  static uint32_t getLAPICError();

private:
  APIC(Tag *TagList);
  // LAPIC stands for Local APIC
  void discoverLAPIC();
  // MADT stands for Multiple APIC Description Table
  void discoverMADT(Tag *TagList);
  // SIV stands for Spurious Interrupt Vector.
  void setupSIV();
  void readAPICID();
  void readAPICMetadata();
  // LVT stands for Local Vector Table.
  void setupLVTEntries();
  static volatile uint32_t *LAPICAddress;
  static InterruptHandler &IH;
};
