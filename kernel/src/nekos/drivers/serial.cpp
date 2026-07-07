#include "nekos/drivers/serial.hpp"
#include "io.h"
#include <stdint.h>

namespace drivers {

Serial::Serial() {
  // Copy pasted from chatgpt. Have no idea what this is.
  outB(PORT + 1, 0x00); // disable interrupts
  outB(PORT + 3, 0x80); // enable DLAB
  outB(PORT + 0, 0x03); // divisor low: 38400 baud
  outB(PORT + 1, 0x00); // divisor high
  outB(PORT + 3, 0x03); // 8 bits, no parity, one stop bit
  outB(PORT + 2, 0xC7); // enable FIFO, clear, 14-byte threshold
  outB(PORT + 4, 0x0B); // IRQs enabled, RTS/DSR set
}

void Serial::writeChar(char Char) {
  while (!(inB(PORT + 5) & 0x20))
    ;
  outB(PORT, Char);
}

void Serial::putChar(char Char) { writeChar(Char); }
void Serial::putString(const char *Str) {
  while (*Str)
    putChar(*Str++);
}
} // namespace drivers
