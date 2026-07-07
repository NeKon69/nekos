#pragma once
#include "kprintf.hpp"
#include <stdint.h>

struct Tag {
  uint32_t Type;
  uint32_t Size;
};

struct Multiboot2Info {
  uint32_t TotalSize;
  uint32_t Reserved;
} __attribute__((packed));
