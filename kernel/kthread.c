/*
* @file kthread.c
* sleep and keyboard_io kernel thread
* bshell is running at keyboard_io kernel thread
*/

#include "stdio.h"
#include "os_stdio.h"
#include "os_sys.h"
#include "os_bits.h"
#include "os_keyboard.h"
#include "os_memory.h"
#include "kthread.h"
#include "os_timer.h"
#include "string.h"
#include "bshell.h"

void thread_lazyman_sleep(int task_id)
{
	struct FIFO32 fifo;
	struct TIMER *tsw;
	int i, fifobuf[128];

	fifo32_init(&fifo, 128, fifobuf);
	tsw = timer_alloc();
	timer_init(tsw, &fifo, 1);
	timer_settime(tsw, 2);

	for (;;) {
		update_mtime_by_pid(task_id, 1);
		asm_cli();
		if (fifo32_status(&fifo) == 0) {
			asm_stihlt();
		} else {
			i = fifo32_get(&fifo);
			asm_sti();
			if (i == 1) {
				timer_settime(tsw, 2);
			}
		}
	}
}

// keyboard i/o threads
void thread_kb_io(int task_id)
{
	struct FIFO32 fifo;
	int i, fifobuf[128];
	struct session sc;
	memset(&sc, 0, sizeof(struct session));
	fifo32_init(&fifo, 128, fifobuf);
	bshell_init(&sc, task_id);
	kb_change_fifo(&fifo);
	for (;;) {
		update_mtime_by_pid(task_id, 1);
		asm_cli();
		if (fifo32_status(&fifo) == 0) {
			asm_stihlt();
		} else {
			i = fifo32_get(&fifo);
			asm_sti();
			kb_input_handling(&sc, i); // shell execution
		}
	}
}	


int hello_main(void);

void thread_events(int task_id)
{
	int event = 0;
	struct thread_message *m = thread_message_init(task_id);
	do {
		event = thread_peek_message(m);			
		if (event > 0) {
		printf("(tid=%d): get event=%d\n", task_id, event);
		hello_main();
		printf("\nend (tid=%d)\n", task_id);
		thread_message_exit(m);
		}
	} while(1);
} 

