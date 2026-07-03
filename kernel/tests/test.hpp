#pragma once

#include "nekos/drivers/serial.hpp"

#include <stddef.h>

namespace tests {

struct TestState {
  size_t Passed;
  size_t Failed;
};

void init(drivers::Serial &Serial);
void runAll();
void summary();
void log(const char *Message);

void pass();
void fail(const char *Expression, const char *File, unsigned Line);

bool expectTrue(bool Condition, const char *Expression, const char *File,
                unsigned Line);

const TestState &state();

} // namespace tests

#define NEKOS_EXPECT_TRUE(Expression)                                          \
  ::tests::expectTrue((Expression), #Expression, __FILE__, __LINE__)
