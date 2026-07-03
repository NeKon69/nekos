#include "nekos/kprintf.hpp"
#include "nekos/drivers/vga.hpp"
#include "stdlib.h"
#include "string.h"
#include <stdarg.h>

drivers::VGA Vga;

// Used for conversions.
char Buffer[33];

void kputs(const char *Char) { Vga.putString(Char); }

void kprintf(const char *Str, ...) {
  va_list Args;
  va_start(Args, Str);

  for (size_t I = 0; Str[I]; I++) {
    if (Str[I] == '%') {
      I++;
      if (Str[I] == '\0') {
        Vga.putChar('%');
        break;
      }
      switch (Str[I]) {
      case 'd':
      case 'i':
        Vga.putString(itoa(va_arg(Args, int), Buffer, 10));
        break;
      case 'u':
        Vga.putString(utoa(va_arg(Args, uint32_t), Buffer, 10));
        break;
      case 'x':
        Vga.putString(utoa(va_arg(Args, uint32_t), Buffer, 16));
        break;
      case 'X':
        Vga.putString(strupr(utoa(va_arg(Args, uint32_t), Buffer, 16)));
        break;
      case 'o':
        Vga.putString(utoa(va_arg(Args, uint32_t), Buffer, 8));
        break;
      case 'c':
        Vga.putChar(va_arg(Args, int));
        break;
      case 's':
        Vga.putString(va_arg(Args, char *));
        break;
      case 'p':
        Vga.putString("0x");
        Vga.putString(
            utoa(reinterpret_cast<uint32_t>(va_arg(Args, void *)), Buffer, 16));
        break;
      case 'z':
        if (Str[I + 1] == 'u') {
          // size_t and unsigned int are the same on my arch heh.
          Vga.putString(utoa(va_arg(Args, size_t), Buffer, 10));
          I++;
        } else {
          Vga.putChar('%');
          Vga.putChar(Str[I]);
        }
        break;
      case '%':
        Vga.putChar('%');
        break;
      default:
        Vga.putChar('%');
        Vga.putChar(Str[I]);
        break;
      }
    } else {
      Vga.putChar(Str[I]);
    }
  }
  va_end(Args);
}

void kclear() { Vga.clear(); }
