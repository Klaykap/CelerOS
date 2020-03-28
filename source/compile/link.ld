/*
*  link.ld
*/
OUTPUT_FORMAT(bin)
ENTRY(start)
SECTIONS
 {
   . = 0x200000;
   .data : { *(.data) }
   .bss  : { *(.bss)  }
 }

