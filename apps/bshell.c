/*
* @file bshell.c
* setup irq for keyboard and timer
* initial the multi-tasking
* startup sleep and keyboard_io kernel thread
* shell is running at keyboard_io kernel thread
*/
#include "kthread.h"
#include "os_stdio.h"
#include "os_stdio2.h"
#include "os_bits.h"
#include "os_pci.h"
#include "bshell.h"
#include "string.h"

#ifndef BOS_VERSION
#define BOS_VERSION "0.2"
#endif

int key_shift = 0;
int key_caps  = 0;

/* character definitions */
#define BOS_CHAR_TAB   '\t'
#define BOS_CHAR_QUOTE '\''
#define BOS_CHAR_MINUS '-'
#define BOS_CHAR_EQUAL '='
#define BOS_CHAR_SLASH '\\'
#define BOS_CHAR_BACKSLASH '/'

#define MAX_KEYTB_SIZE 0x54
/* map for keyboard with 101 key */
char keytable[MAX_KEYTB_SIZE] = {
      //0    1    2     3    4    5    6    7    8    9    A    B    C    D    E   F
	0,   0,   '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', BOS_CHAR_MINUS, BOS_CHAR_EQUAL, 0,   BOS_CHAR_TAB,
	'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', 0,   0,   'a', 's',
	'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', BOS_CHAR_QUOTE, 0,   0,   BOS_CHAR_SLASH, 'z', 'x', 'c', 'v',
	'b', 'n', 'm', ',', '.', BOS_CHAR_BACKSLASH, 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', BOS_CHAR_MINUS, '4', '5', '6', '+', '1',
	'2', '3', '0', '.'
};

char keytable_shift[MAX_KEYTB_SIZE] = {
      //0    1    2     3    4    5    6    7    8    9    A    B    C    D    E   F
	')', ')', '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', 0,   BOS_CHAR_TAB,
	'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', 0,   0,   'A', 'S',
	'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', 0,   0,   '|', 'Z', 'X', 'C', 'V',
	'B', 'N', 'M', '<', '>', '?', 0,   '*', 0,   ' ', 0,   0,   0,   0,   0,   0,
	0,   0,   0,   0,   0,   0,   0,   '7', '8', '9', BOS_CHAR_MINUS, '4', '5', '6', BOS_CHAR_EQUAL, '1',
	'2', '3', '0', '.'
};

static struct MEMMAN *memman = (struct MEMMAN *) MEMMAN_ADDR;
static unsigned int memtotal;

#define KEY_ENTER 0x1C
#define KEY_BS    0x0e /* Back Space */
#define KB_SHIFT_L 1
#define KB_SHIFT_R 2
#define KB_RELEASED 0x80

#define KEY_SHIFT_L 0x2a
#define KEY_SHIFT_R 0x36
#define KEY_TAB     0x0f
#define KEY_SHIFT_RL KEY_SHIFT_L | KB_RELEASED 
#define KEY_SHIFT_RR KEY_SHIFT_R | KB_RELEASED 

/* console definitions */
#define MEM_MB(x) ((x)/1024/1024)



#define new_line(s) { \
	move_cursor(s->cons->x,s->cons->y); \
	clearline(); \
	move_cursor(s->cons->x,s->cons->y); }

int strlen(const char *s);

void command_free(struct session *s)
{
	int freemem = memman_total(memman);
	new_line(s);
	printf("Free/Totoal: %d/%d MB",
		MEM_MB(freemem), 
		MEM_MB(memtotal)); 
	s->cons->y++; /* mem line */
}


#define STR_PROMPT "bos$"

static inline void show_prompt(struct session *s, const char *prompt)
{
	new_line(s);
	puts(prompt);
	s->cons->x+=strlen(prompt) + 1;
}

void command_help(struct session *s)
{
	new_line(s);
	printf( "free  - memory info\n"
		"clear - clear screen\n"
		"ps    - process status\n");
	printf( "uname - show system info\n"
		"type  - dump memory info\n"
		"lspci - dump pci info\n");
	s->cons->y+=6;
}


void command_ps(struct session *s)
{
	struct task *t;
	int i;
	for (i = 0; i < MAX_TASK_NUM ; i++) {
		t = &ptaskctl->tasks[i];
		if (t->flag == TASK_RUN) {
			int pid = getpid_from_task(t);
			new_line(s);
			printf("%d %s - running %d\n", pid, t->processname, getmtime_by_pid(pid));
			s->cons->y++;
		}
	}
}

void command_clear(struct session *s)
{
	clrscr();
	s->cons->y=0;
}

void command_uname(struct session *s)
{
	new_line(s);
	printf("BenOS "BOS_VERSION"\n");
	s->cons->y++; 
}

void hexdump(struct session *s, const char *addr , int len)
{
	int i = len;
	char c = 0;
	const char *p = addr;
	printf("%08x ", (int) addr);
	for (; i > 0; --i, ++p) {
		printf("%02x ", *p);
	}
	p = addr;
	for (; len > 0; --len, ++p) {
		c = *p;
		if ( ('0' <= c && c <= '9') ||
		     ('a' <= c && c <= 'z') ||
		     ('A' <= c && c <= 'Z') || c == ' ') {
			printf("%c", &c);
		} else {
			printf(".");
		}
	}
}

void command_type(struct session *s)
{
	int i = 0;
	const char *arg = s->buf + 5;
	unsigned long tadr = strtoul(arg, NULL, 16);
	const char *p = (const char *) tadr;

	new_line(s);
	printf("type command %x\n", tadr);

	s->cons->y++;
	for  (; i < 16; i++) {
		new_line(s);
		hexdump(s, p, 16);
		p+=16;
		s->cons->y++;
	}
}

void command_lspci(struct session *s)
{
	int r = lspci();
	s->cons->y+=r;
}

typedef struct st_cmdtable {
	const char *cmd; /* name of command */
	int len;  /* length of command */
	void (*pfunc)(struct session *s);
} cmdtable;

static cmdtable command_table[] = {
	{"clear", 5, command_clear},
	{"free", 4, command_free},
	{"help", 4, command_help},
	{"ps", 2, command_ps},
	{"uname", 5, command_uname},
	{"type ", 5, command_type},
	{"lspci", 5, command_lspci},
	{"", 0, NULL}
};

static cmdtable *get_commandhandler(const char *buf)
{
	const char *p = buf;
	int cmdlen = 0;
	while (*p != 0) p++ ;
	cmdlen = p - buf;
	cmdtable *pc = command_table;
	while (pc->len > 0) {
		if (cmdlen >= pc->len && strncmp(buf, pc->cmd, pc->len) == 0) {
			return pc;
		}
		pc++;
	}
	return NULL;
}

void command_exec(struct session *s)
{
	cmdtable *c = NULL;
	s->cons->x=0;
	s->cons->y++;
	if (s->buflen == 0) {
		; /* new line */
	} else if ((c = get_commandhandler(s->buf)) != NULL) {
		c->pfunc(s);
	} else {
		new_line(s);
		printf("invalid command\n");
		s->cons->y++;
	}

	// display prompt string bos$
	s->cons->x = 0;
	show_prompt(s, STR_PROMPT);
}

void bshell_init(struct session *s, int task_id)
{
	move_cursor(s->cons->x,s->cons->y);
	printf("%s started(pid=%d)\n", "bshell", (void *) task_id);
	s->cons->y++;
	show_prompt(s, STR_PROMPT);
}

#define KB_OFFSET 256
#define KB_UPBOUND 512
#define KEY_CTRL_R  0x1d
#define KEY_CTRL_L  0x1f
#define KB_CTRL_R   1
#define KB_CTRL_L   2
int key_ctrl = 0;

static void printf_scancode(struct session *s, int c)
{
	move_cursor(s->cons->x,s->cons->y);
	clearline();
	move_cursor(s->cons->x,s->cons->y);
	printf("code:%x\n", (void *) c, NULL);
	s->cons->x= 0;
	s->cons->y++;
}
/*
 shell run by keyboard_io kernel thread
 */
int kb_input_handling (struct session *s, int c)
{
	if (!(KB_OFFSET <= c && c <KB_UPBOUND))
		return 0;
	
	c-= KB_OFFSET;
	switch(c) {
	case 0xE1: /* skip temp */
	break;
	case KEY_SHIFT_L: /* left shift on */
		key_shift |= KB_SHIFT_L;
	break;
	case KEY_SHIFT_R: /* right shift on */
		key_shift |= KB_SHIFT_R;
	break;
	case KEY_SHIFT_RL: /* left shift off */
		key_shift &= ~KB_SHIFT_L;
	break;
	case KEY_SHIFT_RR: /* right shift off */
		key_shift &= ~KB_SHIFT_R;
	break;
	case KEY_CTRL_R:
		key_ctrl &= ~KB_CTRL_R;
	break;
	case KEY_TAB:
		putc(BOS_CHAR_TAB);
		s->cons->x+=8;
	break;
	default:
		if (c < MAX_KEYTB_SIZE) {
			int ch = 0;
			if ((key_shift & KB_SHIFT_L)
			     || (key_shift & KB_SHIFT_R))
				ch = keytable_shift[c];
			else
				ch = keytable[c];

			if (ch != 0) {
				move_cursor(s->cons->x,s->cons->y);
				putc(ch); s->cons->x++;
				if (s->buflen < MAX_CMD_BUF_SIZE - 1) {
					s->buf[s->buflen++] = ch;
				}
			} else if (c == KEY_BS) { /* process backspace key */
				if (s->buflen > 0) {
					s->buf[--s->buflen] = '\0';
					s->cons->x--;
					move_cursor(s->cons->x,s->cons->y);
					printf(" ");
					move_cursor(s->cons->x,s->cons->y);
				}
			} else if (c == KEY_ENTER) {
				s->buf[s->buflen] = 0;
				command_exec(s);
				s->buflen = 0;
				s->buf[0] = 0;
			} else {
				printf_scancode(s, c);
			}
		} else {
			// printf_scancode(s, c);
		}
	}

	if (s->cons->x > 79) {
		s->cons->x = 0;
		s->cons->y++;
	}

	if (s->cons->y >= 24) {
		s->cons->y = 0;
	}
	return 0;
}

