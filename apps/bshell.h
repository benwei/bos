#ifndef BSHELL_H
#define BSHELL_H

#include "screen.h"

#define MAX_CMD_BUF_SIZE 80

#include "console.h"

struct session {
	int buflen;
	char buf[MAX_CMD_BUF_SIZE];
	int prevbuflen;
	char prevbuf[MAX_CMD_BUF_SIZE];
	unsigned int *pnext; /* next for handling of input buffer */
	unsigned int *ptail; /* proceed location of input buffer */
	console *cons;
};

extern int kb_input_handling (struct session *s, int c);
extern int g_mtime[];

void bshell_init(struct session *s, int taskid);

#define new_line(s) { \
	move_cursor(s->cons->x,s->cons->y); \
	clearline(); \
	move_cursor(s->cons->x,s->cons->y); }

#endif
