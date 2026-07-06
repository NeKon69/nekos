#include "idt.hpp"
#include "idt/idt_helper.hpp"
#include "idt_handler.hpp"
#include "nekos/kprintf.hpp"
#include "test.hpp"

#include <stdint.h>

bool callback_fired = false;
uint32_t callback_error_code = 0;
uint32_t callback_eip = 0;

static void handler_255(const Registers *Regs) {
  callback_fired = true;
  callback_error_code = Regs->ErrorCode;
  callback_eip = Regs->Eip;
}

void test_idt_loads_expected_limit() {
  IDT Idt;
  const tests::idt::IDTR Idtr = tests::idt::readIDTR();
  NEKOS_EXPECT_TRUE(Idtr.Limit == 256 * sizeof(uint64_t) - 1);
}

void test_idt_loads_expected_base() {
  IDT Idt;
  const tests::idt::IDTR Idtr = tests::idt::readIDTR();
  NEKOS_EXPECT_TRUE(Idtr.Base != 0);
  NEKOS_EXPECT_TRUE(Idtr.Base % 8 == 0);
}

void test_idt_entries_are_valid() {
  IDT Idt;
  for (size_t i = 0; i < 256; i++) {
    uint64_t Entry = tests::idt::entryAt(tests::idt::readIDTR(), i);
    kprintf("Entry %u: %x%x ", i, static_cast<uint32_t>(Entry >> 32),
            static_cast<uint32_t>(Entry & 0xFFFF));

    NEKOS_EXPECT_IDT_ENTRY(i);
  }
}

void test_idt_callback_fires() {
  IDT Idt;
  callback_fired = false;
  InterruptHandler::getInterruptHandler().setHandler(255, handler_255);
  asm volatile("int $255");
  NEKOS_EXPECT_TRUE(callback_fired);
}

void test_idt_callback_receives_zero_error_code() {
  IDT Idt;
  callback_error_code = 0;
  InterruptHandler::getInterruptHandler().setHandler(255, handler_255);
  asm volatile("int $255");
  NEKOS_EXPECT_TRUE(callback_error_code == 0);
}

void test_idt_eip_is_correct() {
  IDT Idt;
  callback_eip = 0;
  InterruptHandler::getInterruptHandler().setHandler(255, handler_255);
  asm volatile("int $255");
after_int:
  NEKOS_EXPECT_TRUE(callback_eip == reinterpret_cast<uint32_t>(&&after_int));
}

namespace tests {

void runIDTTests() {
  test_idt_loads_expected_limit();
  test_idt_loads_expected_base();
  test_idt_entries_are_valid();
  test_idt_callback_fires();
  test_idt_callback_receives_zero_error_code();
  test_idt_eip_is_correct();
}

} // namespace tests
