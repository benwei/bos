/*
* @file init.c
* sleep and keyboard_io kernel thread
* bshell is running at keyboard_io kernel thread
*/

#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_sys.h"
#include "os_bits.h"
#include "os_keyboard.h"
#include "os_fifo.h"
#include "os_memory.h"
#include "os_tss.h"
#include "kthread.h"
#include "os_timer.h"
#include "bshell.h"

#define STR_PROMPT "bos$"

int g_mtime[MAX_TASK_NUM];

int getmtime_by_pid(int pid)
{
	return g_mtime[pid];
}

int update_mtime_by_pid(int pid, int offset)
{
	g_mtime[pid] += offset;
	return 0;
}

void thread_lazyman_sleep(int task_id)
{
	struct FIFO32 fifo;
	struct TIMER *tsw, *timer_1s;
	int i, fifobuf[128];
	struct session sc = { 0, 5};
	fifo32_init(&fifo, 128, fifobuf);
	tsw = timer_alloc();
	timer_init(tsw, &fifo, 1);
	timer_settime(tsw, 2);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &fifo, 2);
	timer_settime(timer_1s, 100);
	move_cursor(sc.x,sc.y);
	printf("%s started(pid=%d)\n", (void *) __FUNCTION__, (void *) task_id);
	for (;;) {
		update_mtime_by_pid(task_id, 1);
		asm_cli();
		if (fifo32_status(&fifo) == 0) {
			asm_stihlt();
		} else {
			i = fifo32_get(&fifo);
			asm_sti();
			if (i == 1) {
				task_switch();
				timer_settime(tsw, 2);
			}
		}
	}
}

// keyboard i/o threads
void thread_kb_io(int task_id)
{
	struct FIFO32 fifo;
	struct TIMER *tsw, *timer_1s;
	int i, fifobuf[128];
	struct session sc = { 0, 6};
	fifo32_init(&fifo, 128, fifobuf);
	tsw = timer_alloc();
	timer_init(tsw, &fifo, 1);
	timer_settime(tsw, 2);
	timer_1s = timer_alloc();
	timer_init(timer_1s, &fifo, 2);
	timer_settime(timer_1s, 100);
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
			if (i == 1) {
				task_switch();
				timer_settime(tsw, 2);
			} else {
				kb_input_handling(&sc, i); // shell execution
			}
		}
	}
}

/* functions for app used */
int getpid_from_task(struct task *t)
{
	return t->pid;
}

