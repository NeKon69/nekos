/* Tell the bootloader to look at _start */
ENTRY(_start)
SECTIONS {
  /* Set offset to 2 megabytes */
  . = 2M;
  /* Read-only code */
  .text : ALIGN(4096) {
    *(.multiboot2)
    *(.text*)
  }
  .init_array : ALIGN(4096) {
    __init_array_start = .;
    KEEP(*(SORT_BY_INIT_PRIORITY(.init_array.*)))
    KEEP(*(.init_array))
    __init_array_end = .;
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

