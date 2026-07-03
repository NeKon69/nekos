#include "test.hpp"

#include <stdlib.h>

namespace tests {

void runKprintfTests();

void runAll() {
  runKprintfTests();
  summary();
}

static drivers::Serial *Reporter = nullptr;
static TestState State = {0, 0};

static void write(const char *Str) {
  if (Reporter == nullptr)
    return;
  Reporter->putString(Str);
}

static void writeUnsigned(size_t Value) {
  char Buffer[32];
  utoa(static_cast<uint32_t>(Value), Buffer, 10);
  write(Buffer);
}

void init(drivers::Serial &Serial) {
  Reporter = &Serial;
  State = {0, 0};
  write("NEKOS TEST BEGIN\n");
}

void log(const char *Message) { write(Message); }

void summary() {
  write("NEKOS TEST SUMMARY passed=");
  writeUnsigned(State.Passed);
  write(" failed=");
  writeUnsigned(State.Failed);
  write("\n");

  if (State.Passed == 0 && State.Failed == 0) {
    write("NEKOS NO TESTS RUN\n");
    write("NEKOS TESTS FAILED\n");
  } else if (State.Failed == 0)
    write("NEKOS ALL TESTS PASSED\n");
  else
    write("NEKOS TESTS FAILED\n");
}
void pass() { State.Passed++; }

void fail(const char *Expression, const char *File, unsigned Line) {
  State.Failed++;
  write("NEKOS TEST FAIL ");
  write(File);
  write(":");
  writeUnsigned(Line);
  write(" ");
  write(Expression);
  write("\n");
}

bool expectTrue(bool Condition, const char *Expression, const char *File,
                unsigned Line) {
  if (Condition) {
    pass();
    return true;
  }

  fail(Expression, File, Line);
  return false;
}

const TestState &state() { return State; }

} // namespace tests
