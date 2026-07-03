#include "nekos/kprintf.hpp"
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#include "nekos/drivers/serial.hpp"
#include "test.hpp"

extern "C" void kernel_main() {
  kprintf("Hello how is your day?");
  drivers::Serial Serial;

  tests::init(Serial);
  tests::runAll();
}
