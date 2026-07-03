#pragma once

#include "nekos/kprintf.hpp"
#include "test.hpp"

#include <stddef.h>

namespace tests::vga {

constexpr size_t Width = 80;
constexpr size_t Height = 25;
constexpr size_t CellCount = Width * Height;

char readCharAt(size_t Index);
char readCharAt(size_t Row, size_t Col);
bool textEquals(const char *Expected);
void clearTextMemory();

} // namespace tests::vga

#define NEKOS_VGA_TEST_BEGIN() kclear()

#define NEKOS_VGA_CLEAR() kclear()

#define NEKOS_EXPECT_VGA_TEXT(Expected)                                        \
  NEKOS_EXPECT_TRUE(::tests::vga::textEquals((Expected)))
