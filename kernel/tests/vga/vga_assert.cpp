#include "vga_assert.hpp"

#include <stdint.h>
#include <string.h>

namespace tests::vga {

static volatile uint16_t *const TextMemory =
    reinterpret_cast<volatile uint16_t *>(0xB8000);

char readCharAt(size_t Index) {
  if (Index >= CellCount)
    return '\0';
  return static_cast<char>(TextMemory[Index] & 0xff);
}

char readCharAt(size_t Row, size_t Col) {
  if (Row >= Height || Col >= Width)
    return '\0';
  return readCharAt(Row * Width + Col);
}

bool textEquals(const char *Expected) {
  for (size_t Index = 0; Expected[Index] != '\0'; Index++) {
    if (Index >= CellCount || readCharAt(Index) != Expected[Index])
      return false;
  }

  return readCharAt(strlen(Expected)) == ' ';
}

void clearTextMemory() {
  constexpr uint16_t BlankCell = static_cast<uint16_t>(' ') | (0x0f << 8);

  for (size_t Index = 0; Index < CellCount; Index++)
    TextMemory[Index] = BlankCell;
}

} // namespace tests::vga
