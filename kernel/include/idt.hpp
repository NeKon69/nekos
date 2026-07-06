#pragma once

#include "gdt.hpp"
#include <stdint.h>

class IDT {
public:
  IDT();

private:
  struct IDTEntry {
    uint16_t OffsetLow;
    uint16_t Selector;
    uint8_t Reserved;
    uint8_t Type;
    uint16_t OffsetHigh;
  } __attribute__((packed));

  uint64_t IDTEntries[256];
  size_t EntryCount = 0;
  GDT Gdt;

  void initialize();
  void load(IDTEntry *Idt, uint16_t Size);
};
