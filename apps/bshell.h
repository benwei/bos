#ifndef BSHELL_H
#define BSHELL_H

#include "screen.h"

#define MAX_CMD_BUF_SIZE 80

typedef struct console_st {
	unsigned int vmem_addr;
	unsigned int vmem_current;
	unsigned int vmem_max;
	int x; /* cursor x postion */
	int y; /* cursor y postion */
} console;

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
extern char keytable[];
extern char keytable_shift[];
extern int key_shift;
extern int g_mtime[];

void bshell_init(struct session *s, int taskid);

#endif
