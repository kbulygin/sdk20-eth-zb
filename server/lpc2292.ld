/************************************
#     LPC2292.ld linker script      #
#     for flash based programs      #
#     LPC2292 has 256 Kb flash      #
#     and 16 Kb sram memory         #
************************************/

/* memory map */
MEMORY
{
  flash (rx) : org = 0x00000000, len = 0x00040000
  sram  (rw) : org = 0x40000000, len = 0x00004000
}

/* section descriptions */
SECTIONS
{
  /* .text section - flash executable */
  .text :
  {
    *start.o (.text)
    *(.text)
    *(.glue_7t) *(.glue_7)
  } > flash
  . = ALIGN(4);

  /* .rodata section - read-only data */
  .rodata :
  {
    *(.rodata)
  } > flash
  . = ALIGN(4);

  /* end-of-flash desc */
  _etext = . ;
  PROVIDE (etext = .);

  /* .data section
  #  first reside in flash, then copy to sram
  #  in flash starting at _etext
  #  copying into _data to _edata */
  .data : AT (_etext)
  {
    _data = . ;
    *(.data)
  } > sram
  . = ALIGN(4);

  _edata = . ;
  PROVIDE (edata = .);

  /* .bss section uninit vars
  #  SRAM range need to zeroed */
  .bss :
  {
     _bss = . ;
     *(.bss)
     *(COMMON)
  } > sram
  . = ALIGN(4);
  _ebss = . ;

  _end = . ;
  PROVIDE (end = .);

  /* Stabs debug section */
  .stab             0 : { *(.stab) }
  .stabstr          0 : { *(.stabstr) }
  .stab.excl        0 : { *(.stab.excl) }
  .stab.exclstr     0 : { *(.stab.exclstr) }
  .stab.index       0 : { *(.stab.index) }
  .stab.indexstr    0 : { *(stab.indexstr) }
  .comment          0 : { *(.comment) }

  /* DWARF debug sections */
  /* DWARF 1 */
  .debug            0 : { *(.debug) }
  .line             0 : { *(.line) }
  /* GNU extensions */
  .debug_srcinfo    0 : { *(.debug_srcinfo) }
  .debug_sfnames    0 : { *(.debug_sfnames) }
  /* DWARF 1.1 & 2 */
  .debug_aranges    0 : { *(.debug_aranges) }
  .debug_pubnames   0 : { *(.debug_pubnames) }
  /* DWARF 2 only */
  .debug_info       0 : { *(.debug_info .gnu.linkonce.wi.*) }
  .debug_abbrev     0 : { *(.debug_abbrev) }
  .debug_line       0 : { *(.debug_line) }
  .debug_frame      0 : { *(.debug_frame) }
  .debug_str        0 : { *(.debug_str) }
  .debug_loc        0 : { *(.debug_loc) }
  .debug_macinfo    0 : { *(.debug_macinfo) }
  /* SGI/MIPS DWARF 2 extensions */
  .debug_weaknames  0 : { *(.debug_weaknames) }
  .debug_funcnames  0 : { *(.debug_funcnames) }
  .debug_typenames  0 : { *(.debug_typenames) }
  .debug_varnames   0 : { *(.debug_varnames) }
}
