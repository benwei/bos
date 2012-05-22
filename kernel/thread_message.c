#include "stdio.h"
#include "os_sys.h"
#include "os_bits.h"
#include "os_keyboard.h"
#include "os_memory.h"
#include "kthread.h"
#include "os_timer.h"
#include "string.h"

static struct thread_message thread_messages[THREAD_MESSAGE_MAX];

struct thread_message *
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

void
thread_message_exit(struct thread_message *m)
{
	m->event = 0;
	task_idle(m->task_id);
}

void
thread_wakeup(struct task *t)
{
       ((struct thread_message *)t->env)->event = 1;
}


