#pragma once

#include "test.hpp"

#include <stdint.h>

namespace tests::idt {

struct IDTR {
  uint16_t Limit;
  uint32_t Base;
} __attribute__((packed));

inline IDTR readIDTR() {
  IDTR Result;
  asm volatile("sidt %0" : "=m"(Result));
  return Result;
}

inline uint64_t entryAt(const IDTR &Idtr, size_t Index) {
  return reinterpret_cast<const uint64_t *>(Idtr.Base)[Index];
}

constexpr uint32_t entryOffset(uint64_t Entry) {
  return static_cast<uint32_t>(Entry & 0xFFFF) |
         static_cast<uint32_t>((Entry >> 32) & 0xFFFF0000);
}

constexpr uint16_t entrySelector(uint64_t Entry) {
  return static_cast<uint16_t>((Entry >> 16) & 0xFFFF);
}

constexpr uint8_t entryType(uint64_t Entry) {
  return static_cast<uint8_t>((Entry >> 40) & 0xFF);
}

} // namespace tests::idt
