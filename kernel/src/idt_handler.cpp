#include "idt_handler.hpp"
#include "nekos/kprintf.hpp"

InterruptHandler::InterruptHandler() {}

void InterruptHandler::dumpRegisters(const Registers *Regs) const {
  kprintf("\n===== EXCEPTION %u =====\n", Regs->Vector);
  kprintf("Error code: %u\n", Regs->ErrorCode);
  kprintf("EIP: 0x%x  CS: 0x%x  EFLAGS: 0x%x\n", Regs->Eip, Regs->Cs,
          Regs->EFlags);
  kprintf("EAX: 0x%x  EBX: 0x%x  ECX: 0x%x  EDX: 0x%x\n", Regs->Eax, Regs->Ebx,
          Regs->Ecx, Regs->Edx);
  kprintf("ESI: 0x%x  EDI: 0x%x  EBP: 0x%x  ESP: 0x%x\n", Regs->Esi, Regs->Edi,
          Regs->Ebp, Regs->Esp);
  kprintf("DS: 0x%x\n", Regs->Ds);
}

void InterruptHandler::halt() {
  while (1)
    asm volatile("hlt");
}

void InterruptHandler::setHandler(uint8_t Vector, HandlerFunc Handler) {
  Handlers[Vector] = Handler;
}

void InterruptHandler::handle(const Registers *Regs) {
  if (auto *Func = Handlers[Regs->Vector]) {
    Func(Regs);
  } else {
    dumpRegisters(Regs);
    halt();
  }
}

InterruptHandler &InterruptHandler::getInterruptHandler() {
  static InterruptHandler IH;
  return IH;
}

void idt_handler_stub(const Registers *Regs) {
  InterruptHandler::getInterruptHandler().handle(Regs);
}
