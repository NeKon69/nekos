/* Tell the bootloader to look at _start */
ENTRY(_start)
SECTIONS {
  /* Set offset to 2 megabytes */
  . = 2M;
  /* Read-only code */
  .text : ALIGN(4096) {
    *(.multiboot)
    *(.text*)
  }

  /* ro stands for read-only */
  .rodata : ALIGN(4096) {
    *(.rodata*)
  }

  /* Read-write data (initialized) */
  .data : ALIGN(4096) {
    *(.data*)
  }

  /* Read-write data (uninitialized) */
  .bss : ALIGN(4096) {
    *(COMMON)
    *(.bss*)
  }
}

