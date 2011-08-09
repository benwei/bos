OUTPUT_FORMAT("elf32-little", "elf32-little", "elf32-little")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
	. = 0x00000500;

	. = ALIGN(4);
	.text : 
	{ 	kernel/main.o (.text)
	 	kernel/osfunc.o (.text)
		*(.text) 
	}
	. = ALIGN(4);
	.data : { *(.data) }
}
