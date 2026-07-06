#include "idt.hpp"
#include <stdint.h>

constexpr uint8_t idtInterruptGate = 0xE;
[[maybe_unused]] constexpr uint8_t idtTrapGate = 0xF;

constexpr uint8_t idtPrivilege(uint8_t Priv) { return (Priv & 0b11) << 5; }
constexpr uint8_t idtPresentBit() { return 1 << 7; }

#define X(n) extern "C" void isr_##n();
#include "idt_vectors.inc"
#undef X

IDT::IDT() : Gdt() { initialize(); }

void IDT::initialize() {
  IDTEntry Entries[256];

#define X(n)                                                                   \
  Entries[n].OffsetLow = reinterpret_cast<uint32_t>(isr_##n) & 0xFFFF;         \
  Entries[n].OffsetHigh = reinterpret_cast<uint32_t>(isr_##n) >> 16;           \
  Entries[n].Selector = 0x08;                                                  \
  Entries[n].Reserved = 0;                                                     \
  Entries[n].Type = idtInterruptGate | idtPrivilege(0) | idtPresentBit();
#include "idt_vectors.inc"
#undef X

  load(Entries, 256);
}

void IDT::load(IDTEntry *Entries, uint16_t Count) {
  for (size_t i = 0; i < Count; i++)
    IDTEntries[i] = *reinterpret_cast<uint64_t *>(&Entries[i]);
  struct IDTR {
    uint16_t Limit;
    uint32_t Base;
  } __attribute__((packed)) Idtr;

  Idtr.Limit = Count * sizeof(uint64_t) - 1;
  Idtr.Base = reinterpret_cast<uint32_t>(IDTEntries);

  asm volatile("lidt %0" : : "m"(Idtr));
}
