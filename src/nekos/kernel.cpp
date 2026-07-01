#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#include "stdlib/string.h"
#include <stddef.h>
#include <stdint.h>

enum VGAColor : uint8_t {
  BLACK = 0,
  BLUE = 1,
  GREEN = 2,
  CYAN = 3,
  RED = 4,
  MAGENTA = 5,
  BROWN = 6,
  LIGHT_GREY = 7,
  DARK_GREY = 8,
  LIGHT_BLUE = 9,
  LIGHT_GREEN = 10,
  LIGHT_CYAN = 11,
  LIGHT_RED = 12,
  LIGHT_MAGENTA = 13,
  LIGHT_BROWN = 14,
  WHITE = 15,
};

class VGA {
public:
  VGA() { Clear(); }

  void SetColor(VGAColor FG, VGAColor BG) {
    FGCol = FG;
    BGCol = BG;
  }

  void PutString(const char *Str) {
    size_t len = strlen(Str);
    for (size_t i = 0; i < len; i++) {
      PutChar(Str[i]);
    }
  }
  void PutChar(const char Char) {
    if (Char == '\n') {
      if (++Row == HEIGHT)
        Row = 0;
      Col = 0;
      return;
    }
    VideoMemory[Row * WIDTH + Col] =
        vgaMakeChar(Char, vgaMakeColor(FGCol, BGCol));
    AdvanceCursor();
  }

  void Clear() {
    for (size_t y = 0; y < HEIGHT; y++) {
      for (size_t x = 0; x < WIDTH; x++) {
        VideoMemory[y * WIDTH + x] =
            vgaMakeChar(' ', vgaMakeColor(BGCol, BGCol));
      }
    }
  }

private:
  uint8_t vgaMakeColor(VGAColor FG, VGAColor BG) { return FG | (BG << 4); }

  uint16_t vgaMakeChar(uint8_t Char, uint8_t Color) {
    return Char | (Color << 8);
  }

  void AdvanceCursor() {
    if (++Col == WIDTH) {
      Col = 0;
      if (++Row == HEIGHT) {
        Row = 0;
      }
    }
  }

  volatile uint16_t *VideoMemory = reinterpret_cast<uint16_t *>(0xB8000);
  const uint8_t WIDTH = 80;
  const uint8_t HEIGHT = 25;
  size_t Row = 0;
  size_t Col = 0;
  VGAColor BGCol = BLACK;
  VGAColor FGCol = WHITE;
};

extern "C" void kernel_main() {
  VGA Vga;
  Vga.PutString("Hello os world!!!!\n\n\n\n\nLOLOLOL");
}
