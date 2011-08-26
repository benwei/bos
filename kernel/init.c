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
#include "kthread.h"
#include "bshell.h"

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
	puts("init_pic\n");
	init_pic();
	outb(PIC0_IMR, 0xf8); /* allow PIT, PIC1 and keyboard  */
	outb(PIC1_IMR, 0xef); /* allow mouse */
	outb(0x20, 0x20);
	asm_sti();
	puts("init_pit\n");
	init_pit();
	puts("setup hardware done\n");
}

static struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
static unsigned int memtotal;

void check_memory()
{
	memtotal = memtest(0x00400000, 0xbfffffff);
	memman_init(memman);
	memman_free(memman,0x00400000, memtotal - 0x00400000);
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
	struct TIMER *timer, *timer2, *timer3;
	fifo32_init(&fifo, 128, keybuf);
	struct TSS32 tss_a, tss_c;
	env_init();

	/* keyboard setting */
	init_kb(&fifo, 256);

	i=0;	

	timer = timer_alloc();
	timer_init(timer, &fifo, 10);
	timer_settime(timer, 1000);
	timer2 = timer_alloc();
	timer_init(timer2, &fifo, 3);
	timer_settime(timer2, 200);
	timer3 = timer_alloc();
	timer_init(timer3, &fifo, 1);
	timer_settime(timer3, 50);

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
	task_create((int) &thread_lazyman_sleep, memman, "sleep2");

	struct session sc = {0, 0};

	puts("boot options:");
	for (;;) {
		asm_cli();
		if (fifo32_status(&fifo) == 0) {
			asm_stihlt(); // wake while keyboard input 
			// got a interrupt
		} else { /* scan_code input */
			c=fifo32_get(&fifo);
			asm_sti();
			if (c == 3) { 
				farjmp(0, 4*8);
				puts("2[msec]\n");
			} else if (c == 10) {
				puts("10[sec]\n");
			} else if (c <= 1) {
				if (c != 0) {
					timer_init(timer3, &fifo, 0);
				} else {
					timer_init(timer3, &fifo, 1);
				}
				timer_settime(timer3, 50);
			} else {
				kb_input_handling(&sc, c);
			}
		}
	}
	puts("\nSystem Halted                                    \n");
	_syshalt();
}
