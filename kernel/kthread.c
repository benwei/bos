/*
* @file kthread.c
* sleep and keyboard_io kernel thread
* bshell is running at keyboard_io kernel thread
*/

#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_sys.h"
#include "os_bits.h"
#include "os_keyboard.h"
#include "os_memory.h"
#include "kthread.h"
#include "os_timer.h"
#include "string.h"
#include "bshell.h"

#define STR_PROMPT "bos$"


int getpid_from_task(struct task *t);
int getmtime_by_pid(int pid);
int update_mtime_by_pid(int pid, int offset);

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

static struct thread_message thread_messages[THREAD_MESSAGE_MAX];

static struct thread_message *
thread_message_init(int task_id)
{
	struct task *t = get_task(task_id);
	struct thread_message *m = &thread_messages[task_id];
	m->timer_id = 2;
	m->task_id = task_id;
	m->event = 1;
	fifo32_init(&m->fifo, MAX_FIFO_BUF_SIZE, m->fifobuf);
	m->tsw = timer_alloc();
	timer_init(m->tsw, &m->fifo, m->timer_id);
	timer_settime(m->tsw, m->timer_id);
	t->env = m;
	return m;
}

int
thread_peek_message(struct thread_message *m)
{
	update_mtime_by_pid(m->task_id, 1);
	int counter = 0;
	int evt;

	do {
		asm_cli();
		if (fifo32_status(&m->fifo) == 0) {
			asm_stihlt();
		} else {
			evt = fifo32_get(&m->fifo);
			asm_sti();
			if (evt == m->timer_id) {
				timer_settime(m->tsw, 2);
			} else {
				break;
			}
		}
	} while (counter++ < 2);

	return m->event;
}

void thread_message_exit(struct thread_message *m)
{
	m->event = 0;
	task_idle(m->task_id);
}

void thread_wakeup(struct task *t)
{
	((struct thread_message *)t->env)->event = 1;
}

int task_wait(unsigned int task_id)
{
	struct task *t = get_task(task_id);
	while (t->flag == TASK_RUN){
		;;
	}
	return 0;
}

int hello_main(void);

void thread_events(int task_id)
{
	int event = 0;
	struct thread_message *m = thread_message_init(task_id);
	do {
		event = thread_peek_message(m);			
		if (event > 0) {
		printf("\n(tid=%d): get event=%d\n", task_id, event);
		hello_main();
		printf("\nend (tid=%d)\n", task_id);
		thread_message_exit(m);
		}
	} while(1);
} 

