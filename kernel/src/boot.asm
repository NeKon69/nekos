// Declare constants for the multiboot header.
.set ALIGN,     1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,   1<<1             /* provide memory map */
.set FLAGS,     ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,     0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM,  -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

// Declare a header as in the Multiboot Standard, write the magic number and flags to that header so the bootloader can find it.
.section .multiboot
.align 4
.long MAGIC
.long FLAGS
.long CHECKSUM

// Reserve the memory for the stack.
.section .bss
.balign 16
stack_bottom:
// Increased from 16 KiB to 64 KiB for safe measure.
.skip 64 * 1024 /* 64 KiB */
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    // Make esp point to the top of the stack.
    mov $stack_top, %esp
    // Call the kernel main function.
    call init_global_objects
    call kernel_main
    // Fallback, ideally should never be hit.
    // Disable maskable interrupts.
    cli
    // Halt until an interrupt (non-maskable) occurs, then repeat.
    halt_loop:
        hlt
    jmp halt_loop

// Set size of _start so the debugger and stuff know the size of the function.
.size _start, . - _start


// Apperantly this like reloads the registers with the correct values from the GDT? I don't know.
.global flush_lgdt
.type flush_lgdt, @function
flush_lgdt:
    ljmp $0x08, $flush
flush:
    mov $0x10, %ax
    mov %ax, %ds
    mov %ax, %ss
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    ret
.size flush_lgdt, . - flush_lgdt

// generate stubs 0-255
.set i, -1
.macro gen_stub
.set i, i + 1
.global isr_\@
.type isr_\@, @function
isr_\@:
    // For vectors 8, 10, 11, 12, 13, 14, 17, push only the interrupt number. (error code is already on the stack)
    .if i == 8 || i == 10 || i == 11 || i == 12 || i == 13 || i == 14 || i == 17
        pushl $i
    .else
    // For all other vectors, push 0 as the error code so stack is always in the same state.
        pushl $0
        pushl $i
    .endif
    jmp idt_stub_common
.size isr_\@, . - isr_\@
.endm

.rept 256 
    gen_stub
.endr

.global idt_stub_common
.type idt_stub_common, @function
idt_stub_common:
    pushal                  // save all general-purpose registers
    movw %ds, %ax
    pushl %eax              // save ds

    // switch to kernel data segment
    movw $0x10, %ax         // kernel data segment selector (GDT entry 2)
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs

    pushl %esp              // pass pointer to register frame on stack
    call idt_handler_stub

    popl %eax               // discard argument

    // switch back to user data segment (currently the same as kernel data segment)
    movw (%esp), %ax        // restore ds
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    popl %eax

    popal                   // restore all general-purpose registers

    addl $8, %esp           // drop vector + error code
    iretl                   // pops eflags, cs, eip
.size idt_stub_common, . - idt_stub_common
