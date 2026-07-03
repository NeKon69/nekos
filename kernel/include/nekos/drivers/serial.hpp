#pragma once

#include <stdint.h>

namespace drivers {
class Serial {
public:
  Serial();
  void putChar(char Char);
  void putString(const char *Str);

private:
  static void outB(uint16_t Port, uint8_t Data);
  static uint8_t inB(uint16_t Port);
  void writeChar(char Char);

  static const uint16_t PORT = 0x3f8;
};
} // namespace drivers
