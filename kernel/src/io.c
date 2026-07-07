#include "io.h"

uint8_t inB(uint16_t Port) {
  uint8_t result;
  __asm__ volatile("inb %1, %0" : "=a"(result) : "Nd"(Port));
  return result;
}

void outB(uint16_t Port, uint8_t Data) {
  __asm__ volatile("outb %0, %1" : : "a"(Data), "Nd"(Port));
}

uint16_t inW(uint16_t Port) {
  uint16_t result;
  __asm__ volatile("inw %1, %0" : "=a"(result) : "Nd"(Port));
  return result;
}

void outW(uint16_t Port, uint16_t Data) {
  __asm__ volatile("outw %0, %1" : : "a"(Data), "Nd"(Port));
}

void outl(uint16_t Port, uint32_t Data) {
  __asm__ volatile("outl %0, %1" : : "a"(Data), "Nd"(Port));
}
