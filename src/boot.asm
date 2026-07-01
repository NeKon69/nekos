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
.skip 16 * 1024 /* 16 KiB */
stack_top:

.section .text
.global _start
.type _start, @function
_start:
    // Make esp point to the top of the stack.
    mov $stack_top, %esp
    // Call the kernel main function.
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
