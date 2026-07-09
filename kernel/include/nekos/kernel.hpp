#pragma once
#include <stdint.h>

struct MB2Tag {
  uint32_t Type;
  uint32_t Size;
};

struct Multiboot2Info {
  uint32_t TotalSize;
  uint32_t Reserved;
} __attribute__((packed));
