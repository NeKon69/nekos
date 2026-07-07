#ifndef NEKOS_IO_H
#define NEKOS_IO_H

#include "extern_c.h"
#include <stdint.h>

EXTERN_C_BEGIN
uint8_t inB(uint16_t Port);
void outB(uint16_t Port, uint8_t Data);
uint16_t inW(uint16_t Port);
void outW(uint16_t Port, uint16_t Data);
void outl(uint16_t Port, uint32_t Data);
EXTERN_C_END

#endif
