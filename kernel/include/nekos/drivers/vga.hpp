#pragma once

#include <stddef.h>
#include <stdint.h>

namespace drivers {
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
  VGA();
  VGA(VGAColor FG, VGAColor BG);

  void setColor(VGAColor FG, VGAColor BG);
  void putString(const char *Str);
  void putChar(char Char);
  void clear();

private:
  uint8_t vgaMakeColor(VGAColor FG, VGAColor BG);
  uint16_t vgaMakeChar(uint8_t Char, uint8_t Color);
  void advanceCursor();

  static volatile uint16_t *VideoMemory;
  static constexpr uint8_t WIDTH = 80;
  static constexpr uint8_t HEIGHT = 25;
  size_t Row = 0;
  size_t Col = 0;
  VGAColor BGCol = BLACK;
  VGAColor FGCol = WHITE;
  bool ScreenFull = false;
};
} // namespace drivers
