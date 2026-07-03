#include "nekos/kprintf.hpp"
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#if NEKOS_KERNEL_TESTS
#include "nekos/drivers/serial.hpp"
#include "test.hpp"
#endif

extern "C" void kernel_main() {
#if NEKOS_KERNEL_TESTS
  drivers::Serial Serial;

  tests::init(Serial);
  tests::runAll();
#else
  kprintf("Hello how is your day?");
#endif
}
