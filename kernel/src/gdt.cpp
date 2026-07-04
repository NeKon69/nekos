#include "gdt.hpp"
#include <stdint.h>

// AccessByte helpers (bits 0-7 of the byte, maps to bits 40-47 of entry)

/// \param Value true = segment has been accessed (CPU sets this automatically)
constexpr uint8_t gdtAccessed(bool Value) { return Value << 0; }
/// \param Value true = segment is writable (data) or readable (code)
constexpr uint8_t gdtReadWrite(bool Value) { return Value << 1; }
/// \param Value true = segment is executable (code segment)
constexpr uint8_t gdtExecutable(bool Value) { return Value << 3; }
/// \param Value true = code/data segment, false = system segment (TSS, etc.)
constexpr uint8_t gdtSystem(bool Value) { return Value << 4; }
/// \param Priv privilege level 0-3 (0 = kernel, 3 = user)
constexpr uint8_t gdtPrivilege(uint8_t Priv) { return (Priv & 0x03) << 5; }
/// \param Value true = segment is present in memory, false = not present
constexpr uint8_t gdtPresent(bool Value) { return Value << 7; }

// Flags helpers (bits 0-3 of the byte, maps to bits 52-55 of entry)

/// \param Value available for OS use (CPU ignores this bit)
constexpr uint8_t gdtAvailable(bool Value) { return Value << 0; }
/// \param Value true = 64-bit segment (long mode)
constexpr uint8_t gdtLong(bool Value) { return Value << 1; }
/// \param Value true = 32-bit segment, false = 16-bit segment
constexpr uint8_t gdtSize(bool Value) { return Value << 2; }
/// \param Value true = limit scaled by 4KB, false = limit in bytes
constexpr uint8_t gdtGranularity(bool Value) { return Value << 3; }

GDT::GDT() { initialize(); }

uint64_t GDT::buildEntry(const GDTEntry &Entry) {
  // Copied from chatgpt. Fuck this man...
  uint64_t Result = 0;
  Result |= (Entry.Limit & 0xFFFF);        // Bits 0-15
  Result |= ((Entry.Base & 0xFFFF) << 16); // Bits 16-31
  // High 32 bits (Bits 32 - 63)
  Result |=
      (((static_cast<uint64_t>(Entry.Base) >> 16) & 0xFF) << 32); // Bits 32-39
  Result |=
      ((static_cast<uint64_t>(Entry.AccessByte) & 0xFF) << 40); // Bits 40-47
  Result |=
      (((static_cast<uint64_t>(Entry.Limit) >> 16) & 0x0F) << 48); // Bits 48-51
  Result |= ((static_cast<uint64_t>(Entry.Flags) & 0x0F) << 52);   // Bits 52-55
  Result |=
      (((static_cast<uint64_t>(Entry.Base) >> 24) & 0xFF) << 56); // Bits 56-63

  return Result;
}

void GDT::load(const GDTEntry *Gdt, size_t Size) {
  Size = Size > 5 ? 5 : Size;
  for (size_t i = 0; i < Size; i++) {
    GdtEntries[i] = buildEntry(Gdt[i]);
  }
  EntryCount = Size;

  struct GDTR {
    uint16_t Limit;
    uint32_t Base;
  } __attribute__((packed)) Gdtr;

  Gdtr.Limit = Size * sizeof(uint64_t) - 1;
  Gdtr.Base = reinterpret_cast<uint32_t>(GdtEntries);

  asm volatile("lgdt %0" : : "m"(Gdtr));
}

void GDT::initialize() {
  GDTEntry Entries[3];

  // Null entry
  Entries[0].Limit = 0;
  Entries[0].Base = 0;
  Entries[0].AccessByte = 0;
  Entries[0].Flags = 0;

  // Code segment
  Entries[1].Limit = 0xFFFFF;
  Entries[1].Base = 0;
  Entries[1].AccessByte = gdtSystem(true) | gdtPresent(true) |
                          gdtExecutable(true) | gdtPrivilege(0);
  Entries[1].Flags = gdtSize(true) | gdtGranularity(true);

  // Data segment
  Entries[2].Limit = 0xFFFFF;
  Entries[2].Base = 0;
  Entries[2].AccessByte =
      gdtSystem(true) | gdtPresent(true) | gdtReadWrite(true) | gdtPrivilege(0);
  Entries[2].Flags = gdtSize(true) | gdtGranularity(true);

  load(Entries, 3);
}
