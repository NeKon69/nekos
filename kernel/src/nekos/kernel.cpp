#include "nekos/kernel.hpp"
#include "apic.hpp"
#include "idt.hpp"
#include "nekos/kprintf.hpp"
#if !defined(__i386__)
#error "This needs to be compiled with a ix86-elf compiler"
#endif

#if NEKOS_KERNEL_TESTS
#include "nekos/drivers/serial.hpp"
#include "test.hpp"
#endif

extern "C" void kernel_main(Multiboot2Info *Info) {
  IDT Idt;
#if NEKOS_KERNEL_TESTS
  kprintf("Hello how is your day?\n");
  // Construct APIC.
  APIC::getAPIC(reinterpret_cast<Tag *>(reinterpret_cast<char *>(Info) +
                                        sizeof(Multiboot2Info)));
  // drivers::Serial Serial;

  // tests::init(Serial);
  // tests::runAll();
#else
#endif
  while (1)
    asm volatile("hlt");
}
