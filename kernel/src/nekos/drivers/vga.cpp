#include "nekos/drivers/vga.hpp"

namespace drivers {
VGA::VGA() { clear(); }

VGA::VGA(VGAColor FG, VGAColor BG) {
  setColor(FG, BG);
  clear();
}

void VGA::setColor(VGAColor FG, VGAColor BG) {
  FGCol = FG;
  BGCol = BG;
}

void VGA::putString(const char *Str) {
  size_t len = 0;
  while (Str[len])
    len++;
  for (size_t i = 0; i < len; i++) {
    putChar(Str[i]);
  }
}

void VGA::putChar(char Char) {
  if (Char == '\n') {
    if (++Row == HEIGHT) {
      Row = 0;
      clear();
    }
    Col = 0;
    return;
  }
  VideoMemory[Row * WIDTH + Col] =
      vgaMakeChar(Char, vgaMakeColor(FGCol, BGCol));
  advanceCursor();
}

void VGA::clear() {
  for (size_t y = 0; y < HEIGHT; y++) {
    for (size_t x = 0; x < WIDTH; x++) {
      VideoMemory[y * WIDTH + x] = vgaMakeChar(' ', vgaMakeColor(BGCol, BGCol));
    }
  }
  Col = 0;
  Row = 0;
}

uint8_t VGA::vgaMakeColor(VGAColor FG, VGAColor BG) { return FG | (BG << 4); }

uint16_t VGA::vgaMakeChar(uint8_t Char, uint8_t Color) {
  return Char | (Color << 8);
}

void VGA::advanceCursor() {
  if (++Col == WIDTH) {
    Col = 0;
    if (++Row == HEIGHT) {
      Row = 0;
      clear();
    }
  }
}

volatile uint16_t *VGA::VideoMemory = reinterpret_cast<uint16_t *>(0xB8000);
} // namespace drivers
