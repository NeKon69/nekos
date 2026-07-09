#include "nekos/interrupts/io_apic.hpp"

namespace nekos::interrupts {
constexpr uint8_t IOAPICVER = 0x01;

IOAPIC::IOAPIC(uint32_t Address)
    : Base(reinterpret_cast<volatile uint32_t *>(Address)) {}

uint32_t IOAPIC::read(uint8_t Reg) const {
  Base[0] = Reg;
  return Base[4];
}

void IOAPIC::write(uint8_t Reg, uint32_t Data) const {
  Base[0] = Reg;
  Base[4] = Data;
}

uint32_t IOAPIC::readVersion() const { return read(IOAPICVER); }
} // namespace nekos::interrupts
