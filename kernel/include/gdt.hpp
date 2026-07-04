#pragma once

#include <stddef.h>
#include <stdint.h>

#if NEKOS_KERNEL_TESTS
namespace tests::gdt {
struct GDTAccess;
}
#endif

class GDT {
public:
  GDT();

private:
  struct GDTEntry {
    uint32_t Limit;
    uint32_t Base;
    uint8_t AccessByte;
    uint8_t Flags;
  } __attribute__((packed));

  uint64_t GdtEntries[5];
  size_t EntryCount = 0;

#if NEKOS_KERNEL_TESTS
  friend struct tests::gdt::GDTAccess;
#endif

  void initialize();
  void load(const GDTEntry *Gdt, size_t Size);
  static uint64_t buildEntry(const GDTEntry &Entry);
};
