#pragma once

#include <stdint.h>

namespace drivers {
class Serial {
public:
  Serial();
  void putChar(char Char);
  void putString(const char *Str);

private:
  void writeChar(char Char);

  static const uint16_t PORT = 0x3f8;
};
} // namespace drivers
