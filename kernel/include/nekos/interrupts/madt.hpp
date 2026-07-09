#pragma once

#include "nekos/kernel.hpp"
#include <stdint.h>

namespace nekos::interrupts {
// MADT stands for Multiple APIC Description Table
struct MADTRecord {
  uint8_t Type;
  uint8_t Length;
} __attribute__((packed));

// MADT Record structure for IO APIC
struct MADTIOAPIC : MADTRecord {
  uint8_t ID;
  uint8_t Reserved;
  uint32_t Address;
  uint32_t GlobalBaseIRQ;
} __attribute__((packed));

uint32_t *discoverMADT(MB2Tag *TagList);
MADTIOAPIC *findIOAPIC(uint32_t *MADT);
} // namespace nekos::interrupts
