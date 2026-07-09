#pragma once

#include <stdint.h>

namespace nekos::interrupts {
class IOAPIC {
public:
  explicit IOAPIC(uint32_t Address);

  uint32_t read(uint8_t Reg) const;
  void write(uint8_t Reg, uint32_t Data) const;
  uint32_t readVersion() const;

private:
  volatile uint32_t *Base;
};
}
