/*
* @file init.c
* setup irq for keyboard and timer
* initial the multi-tasking to start kernel threads
*/

#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_sys.h"
#include "os_bits.h"
#include "os_keyboard.h"
#include "os_fifo.h"
#include "os_memory.h"
#include "os_tss.h"
#include "os_mtask.h"
#include "os_timer.h"
#include "os_pci.h"
#include "kthread.h"
#include "bshell.h"
#include "memory.h"

/* pointer for global descriptor table */
struct SEGMENT_DESCRIPTOR *gdt = (struct SEGMENT_DESCRIPTOR *) ADR_GDT;

/* boot message */
#define msg_boot32ok "Enter Protected Mode Successfully\n"

int floppy = 0;

void setup_hw(void)
{
	asm_cli();
	puts("init gdt and idt\n");
	init_gdtidt();
	puts("init pic\n");
	init_pic();
	outb(PIC0_IMR, 0xf8); /* allow PIT, PIC1 and keyboard  */
	outb(PIC1_IMR, 0xef); /* allow mouse */
	outb(0x20, 0x20);
	asm_sti();
	puts("init pit\n");
	init_pit();
	puts("setup hardware done\n");
}

static struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;

void check_memory()
{
	calc_e820();
	/* if calc_e820 can not work,
	   we may use dirty way to test memory size.*/
#if HAVE_MEMTEST
	mem_upbound_size = memtest(0x00400000, 0xbfffffff);
#endif
	memman_init(memman);
	memman_free(memman,0x00400000, mem_upbound_size);
}

/* move_cursor */

void env_init()
{
	clrscr();
	move_cursor(0,0);
	setup_hw();
	puts("Ben OS v"BOS_VERSION" starting...\n");
	puts(msg_boot32ok);
	check_memory();
	floppy = get_floppy_info();
}


void _benmain(void)
{
	int i;
	int c = 0;
	int keybuf[128]={0};
	struct FIFO32 fifo;
	struct TIMER *timer;
	fifo32_init(&fifo, 128, keybuf);
	struct TSS32 tss_a, tss_c;
	env_init();

	/* keyboard setting */
	init_kb(&fifo, 256);

	i=0;	

	timer = timer_alloc();
	timer_init(timer, &fifo, 1);
	timer_settime(timer, 50);

	tss_a.ldtr = 0;
	tss_a.iomap = 0x40000000;
	tss_c.ldtr = 0;
	tss_c.iomap = 0x40000000;
	set_segmdesc(gdt + 3, 103, (int) &tss_a, AR_TSS32);

	task_init(memman);

	/* Unfortunate try the switch back to 3*8 (_benmain) with farjmp but not working.
	* And the reason still unknown.
	* So that I use another thread_kb_io() to catch the keyboard interrupt.
	*/

	load_tr(3 * 8); // record current working task to tss_a
	task_create((int) &thread_kb_io, memman, "bshell");

	/* strange issue encountered if run pci_init at the setup_hw() */
	puts("pci scan bus\n");
	pci_init();
	for (;;) {
		asm_cli();
		if (fifo32_status(&fifo) == 0) {
			asm_stihlt(); // wake while keyboard input 
			// got a interrupt
		} else { /* scan_code input */
			c=fifo32_get(&fifo);
			asm_sti();
			if (c == 1) { 
				farjmp(0, 4*8);
			} else {
				puts("disabled boot options.\n");
			}
		}
	}
	puts("\nSystem Halted                                    \n");
	_syshalt();
}
