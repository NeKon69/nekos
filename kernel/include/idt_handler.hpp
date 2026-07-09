#pragma once

#include <stdint.h>

struct Registers {
  uint32_t Ds;
  uint32_t Edi;
  uint32_t Esi;
  uint32_t Ebp;
  uint32_t Esp;
  uint32_t Ebx;
  uint32_t Edx;
  uint32_t Ecx;
  uint32_t Eax;
  uint32_t Vector;
  uint32_t ErrorCode;
  uint32_t Eip;
  uint32_t Cs;
  uint32_t EFlags;
} __attribute__((packed));

static_assert(sizeof(Registers) == 56,
              "Registers struct size must match asm stack frame");

extern "C" void idt_handler_stub(const Registers *Regs);

class InterruptHandler {
public:
  using HandlerFunc = void (*)(const Registers *);
  void setHandler(uint8_t Vector, HandlerFunc Handler);
  [[noreturn]] static void halt();
  static InterruptHandler &getInterruptHandler();

private:
  InterruptHandler();
  void dumpRegisters(const Registers *Regs) const;
  void handle(const Registers *Regs);

  friend void idt_handler_stub(const Registers *Regs);

  HandlerFunc Handlers[256] = {};
};
