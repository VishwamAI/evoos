/* Define the memory layout for the kernel */
MEMORY
{
  /* Define the region of memory for the code and read-only data */
  FLASH (rx) : ORIGIN = 0x00100000, LENGTH = 256K

  /* Define the region of memory for the data and bss sections */
  RAM (rwx) : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define the entry point of the kernel */
ENTRY(_start)

/* Define the sections */
SECTIONS
{
  /* Place the text (code) section at the beginning of the FLASH region */
  .text :
  {
    *(.text)
  } > FLASH

  /* Place the data section at the beginning of the RAM region */
  .data :
  {
    *(.data)
  } > RAM

  /* Place the bss section after the data section in the RAM region */
  .bss :
  {
    *(.bss)
  } > RAM
}
