#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

const char *Alphabet = "0123456789abcdefghijklmnopqrstuvwxyz";

char *UToAImpl(uint32_t Value, char *Str [[clang::lifetimebound]], int Base) {
  if (Value == 0)
    *Str++ = '0';
  while (Value > 0) {
    int Temp = Value % Base;
    Value /= Base;
    *Str++ = Alphabet[Temp];
  }
  return Str;
}

char *itoa(int value, char *str, int base) {
  char *Ret = str;
  unsigned int UVal = (value < 0) ? -((unsigned int)value) : value;
  str = UToAImpl(UVal, str, base);
  if (value < 0)
    *str++ = '-';
  *str = '\0';
  strrev(Ret);
  return Ret;
}

char *utoa(uint32_t value, char *str, int base) {
  char *Ret = str;
  str = UToAImpl(value, str, base);
  *str = '\0';
  strrev(Ret);
  return Ret;
}
