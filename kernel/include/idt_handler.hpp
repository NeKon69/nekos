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
};

class InterruptHandler {
public:
  InterruptHandler();
  void handle(const Registers *Regs);

private:
  void dumpRegisters(const Registers *Regs);
  void halt();
};

extern "C" void idt_handler_stub(const Registers *Regs);
