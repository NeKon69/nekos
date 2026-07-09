#include "nekos/kernel.hpp"
#include "idt.hpp"
#include "nekos/interrupts/apic.hpp"
#include "nekos/kprintf.hpp"
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#if NEKOS_KERNEL_TESTS
#include "nekos/drivers/serial.hpp"
#include "test.hpp"
#endif

extern "C" void kernel_main(Multiboot2Info *Info) {
  // Construct APIC. This is a smoke test.
  APIC &Apic = APIC::getAPIC(reinterpret_cast<MB2Tag *>(
      reinterpret_cast<char *>(Info) + sizeof(Multiboot2Info)));
#if NEKOS_KERNEL_TESTS
  kprintf("Hello how is your day?\n");
  drivers::Serial Serial;

  tests::init(Serial);
  tests::runAll();
#else
  IDT Idt;
#endif
  while (1)
    asm volatile("hlt");
}
