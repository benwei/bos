OUTPUT_FORMAT("elf32-little", "elf32-little", "elf32-little")
OUTPUT_ARCH(i386)
ENTRY(_start)
SECTIONS
{
	. = 0x00001000;
	. = ALIGN(4);
	.text : 
	{ 	kernel/main.o (.text)
	 	kernel/osfunc.o (.text)
		*(.hwsetup.text)
		*(.text) 
	}
	. = ALIGN(4);
	.data :
	{	*(.hwsetup.data)
		*(.data)
	}
}
