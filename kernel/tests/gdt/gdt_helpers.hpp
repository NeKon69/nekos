#pragma once

#include "gdt.hpp"
#include "test.hpp"

#include <stdint.h>

namespace tests::gdt {

struct GDTAccess {
  static uint64_t buildEntry(uint32_t Limit, uint32_t Base, uint8_t AccessByte,
                             uint8_t Flags) {
    const GDT::GDTEntry Entry{
        .Limit = Limit,
        .Base = Base,
        .AccessByte = AccessByte,
        .Flags = Flags,
    };

    return GDT::buildEntry(Entry);
  }
};

constexpr uint8_t Present = 0x80;
constexpr uint8_t CodeData = 0x10;
constexpr uint8_t Executable = 0x08;
constexpr uint8_t Writable = 0x02;
constexpr uint8_t Accessed = 0x01;
constexpr uint8_t Size32 = 0x04;
constexpr uint8_t Granularity4K = 0x08;

constexpr uint8_t KernelCodeAccess =
    Present | CodeData | Executable | Writable | Accessed;
constexpr uint8_t KernelDataAccess = Present | CodeData | Writable | Accessed;
constexpr uint8_t FlatSegmentFlags = Size32 | Granularity4K;

constexpr uint64_t NullEntry = 0x0000000000000000;
constexpr uint64_t KernelCodeEntry = 0x00CF9B000000FFFF;
constexpr uint64_t KernelDataEntry = 0x00CF93000000FFFF;

constexpr uint32_t FlatSegmentLimit = 0xFFFFF;
constexpr uint32_t FlatSegmentBase = 0;

struct GDTR {
  uint16_t Limit;
  uint32_t Base;
} __attribute__((packed));

inline GDTR readGDTR() {
  GDTR Result;
  asm volatile("sgdt %0" : "=m"(Result));
  return Result;
}

inline const uint64_t *table(const GDTR &Gdtr) {
  return reinterpret_cast<const uint64_t *>(Gdtr.Base);
}

inline bool gdtrEquals(const GDTR &Gdtr, uint32_t Base, uint16_t Limit) {
  return Gdtr.Base == Base && Gdtr.Limit == Limit;
}

inline bool gdtrLimitEquals(const GDTR &Gdtr, uint16_t Limit) {
  return Gdtr.Limit == Limit;
}

inline bool gdtrBaseEquals(const GDTR &Gdtr, uint32_t Base) {
  return Gdtr.Base == Base;
}

inline bool entryEquals(const GDTR &Gdtr, size_t Index, uint64_t Expected) {
  return table(Gdtr)[Index] == Expected;
}

inline bool entryAccessed(const GDTR &Gdtr, size_t Index) {
  return (table(Gdtr)[Index] & (static_cast<uint64_t>(Accessed) << 40)) != 0;
}

} // namespace tests::gdt

#define NEKOS_EXPECT_GDTR(Base, Limit)                                         \
  do {                                                                         \
    const ::tests::gdt::GDTR Gdtr = ::tests::gdt::readGDTR();                  \
    NEKOS_EXPECT_TRUE(::tests::gdt::gdtrEquals((Gdtr), (Base), (Limit)));      \
  } while (false)

#define NEKOS_EXPECT_GDTR_LIMIT(Limit)                                         \
  do {                                                                         \
    const ::tests::gdt::GDTR Gdtr = ::tests::gdt::readGDTR();                  \
    NEKOS_EXPECT_TRUE(::tests::gdt::gdtrLimitEquals((Gdtr), (Limit)));         \
  } while (false)

#define NEKOS_EXPECT_GDTR_BASE(Base)                                           \
  do {                                                                         \
    const ::tests::gdt::GDTR Gdtr = ::tests::gdt::readGDTR();                  \
    NEKOS_EXPECT_TRUE(::tests::gdt::gdtrBaseEquals((Gdtr), (Base)));           \
  } while (false)

#define NEKOS_EXPECT_GDT_ENTRY(Index, Expected)                                \
  do {                                                                         \
    const ::tests::gdt::GDTR Gdtr = ::tests::gdt::readGDTR();                  \
    NEKOS_EXPECT_TRUE(::tests::gdt::entryEquals((Gdtr), (Index), (Expected))); \
  } while (false)

#define NEKOS_EXPECT_GDT_ENTRY_ACCESSED(Index)                                 \
  do {                                                                         \
    const ::tests::gdt::GDTR Gdtr = ::tests::gdt::readGDTR();                  \
    NEKOS_EXPECT_TRUE(::tests::gdt::entryAccessed((Gdtr), (Index)));           \
  } while (false)
