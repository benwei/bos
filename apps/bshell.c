/*
* @file bshell.c
* setup irq for keyboard and timer
* initial the multi-tasking
* startup sleep and keyboard_io kernel thread
* shell is running at keyboard_io kernel thread
*/
#include "kthread.h"
#include "os_stdio.h"
#include "stdio.h"
#include "os_bits.h"
#include "os_pci.h"
#include "bshell.h"
#include "string.h"
#include "keymap.h"
#include "memory.h"
#include "net.h"
#include "malloc.h"
#include "os_mtask.h"
#include "os_timer.h"
#include "kproc.h"

static int key_shift = 0;
static int key_caps  = 0;

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


#define MEM_MB(x) ((x)/1024/1024)


void dump_e820(struct session *s)
{
	int i;
	const char *msg;
	e820_t *e = e820_info;
	for (i = 0; i < e820_smap_num; i++, e++) {
		if (e->type == E820_ADDR_RANGE){
			msg = "Free";
		} else
			msg = "Reserved";
		if (e->addr_high == 0 && e->len_high == 0) {
		printf(" %0x | %0x | %s(%d)\n",
			 e->addr_low,
			 e->len_low,
			 msg, e->type);
		} else {
		printf(" %0x%0x | %0x%0x | %s(%d)\n",
			 e->addr_high, e->addr_low,
			 e->len_high, e->len_low,
			 msg, e->type);
		}
	}
}

void command_test(struct session *s)
{
	int *i = (int *)malloc(sizeof(int));
        if (i) {
		printf("test malloc %x\n", i);
	}

}

void command_free(struct session *s)
{
	int freemem = memman_total((struct MEMMAN *) MEMMAN_ADDR);
	dump_e820(s);
	printf("Free/Total: %d/%d MB\n",
		MEM_MB(freemem), MEM_MB(mem_upbound_size));
}

#define STR_PROMPT "bos$ "

static inline void show_prompt(struct session *s, const char *prompt)
{
	puts(prompt);
}

void command_ps(struct session *s)
{
	const char *msg_state[] = {"running", "stop", "idle"};
	
	struct proc mproc;
	proc_init(&mproc);
	struct ps mps = {0};
	while(proc_read_ps(&mproc, &mps)){
		printf("%d %s - %s %d\n",
			mps.pid,
			mps.name,
			msg_state[mps.state],
			mps.mtime);
	}
}

void command_clear(struct session *s)
{
	clear_screen();
}

void command_uname(struct session *s)
{
	printf("BenOS "BOS_VERSION"\n");
}

void hexdump(struct session *s, const char *addr , int len)
{
	int i = len;
	char c = 0;
	const char *p = addr;
	printf("%08x ", (int) addr);
	for (; i > 0; --i, ++p) {
		printf("%02x ", *p);
		if (i == 9) {
			putc(' ');
		}
	}
	p = addr;
	for (; len > 0; --len, ++p) {
		c = *p;
		if ( ('0' <= c && c <= '9') ||
		     ('a' <= c && c <= 'z') ||
		     ('A' <= c && c <= 'Z') || c == ' ') {
			putc(c);
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

	for  (; i < 16; i++) {
		hexdump(s, p, 16);
		p+=16;
		putc('\n');
	}
}

void command_lspci(struct session *s)
{
	lspci();
}

void user_div_zero(void);

void command_ud2() {
	__asm __volatile("ud2");	
}

void command_divzero(struct session *s)
{
	user_div_zero();
}

void command_ticks()
{
	printf("ticks: %d\n", sys_get_ticks());
}

int hello_main(void);
void command_hello(struct session *s)
{
	hello_main();
}

void command_run(struct session *s) {
	task_start(2);
	/* blocking util task stopped */
	task_wait(2);
}

void command_net(struct session *s);
void command_net_tx(struct session *s);


typedef struct st_cmdtable {
	const char *cmd; /* name of command */
	int len;  /* length of command */
	void (*pfunc)(struct session *s);
	const char *desc;
} cmdtable;

static void command_help(struct session *s);

static cmdtable command_table[] = {
	{"clear",  5, &command_clear, "clear screen"},
	{"free",   4, &command_free,  "memory info"},
	{"help",   4, &command_help,  "help info" },
	{"ps",     2, &command_ps,    "process status" },
	{"uname",  5, &command_uname, "show system info" },
	{"type ",  5, &command_type,  "type memory info" },
	{"lspci",  5, &command_lspci, "list pci info" },
	{"divzero",7, &command_divzero,"idt testing" },
	{"ud2",    3, &command_ud2,   "idt testing" },
	{"hello",  5, &command_hello, "idt hello" },
	{"run",    3, &command_run,   "run events" },
	{"netsend",7, &command_net_tx,"test ethernet send" },
	{"net",    3, &command_net,   "list ethernet info" },
	{"test",   4, &command_test,  "test vfs info" },
	{"ticks",  5, &command_ticks,  "get ticks" },
	{"", 0, NULL}
};

static void
command_help(struct session *s)
{
	cmdtable *pc = command_table;
	while (pc->len != 0) {
		printf( "%9s - %s\n", pc->cmd, pc->desc);
		pc++;
	}
}

static cmdtable *
get_commandhandler(const char *buf)
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
	putc('\n');
	if (s->buflen) {
		if (s->buflen
			&& (c = get_commandhandler(s->buf)) != NULL) {
			c->pfunc(s);
		} else {
			printf("invalid command\n");
		}
	}
	show_prompt(s, STR_PROMPT);
}

void bshell_init(struct session *s, int task_id)
{
	//printf("%s started(pid=%d)\n", "bshell", (void *) task_id);
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
	printf("code:%x\n", (void *) c, NULL);
}

/* key's scan code with e0 */
#define KEY_UP    0x48
#define KEY_LEFT  0x4B
#define KEY_DOWN  0x50
#define KEY_RIGHT 0x4D

#define KEY_HOME  0x47
#define KEY_PGUP  0x49
#define KEY_DEL   0x53
#define KEY_END   0x4F
#define KEY_PGDN  0x51

/* handling the scan code with e0 */
void control_key_handling(struct session *s, int c)
{
	switch(c) {
	case KEY_UP:
		s->buflen = s->prevbuflen;
		strcpy(s->buf, s->prevbuf);
		s->buf[s->buflen] = 0;
		printf("%s", s->buf);
	break;
	case KEY_LEFT:
		printf("KEY_LEFT\n");
		s->cons->y++;
	break;
	case KEY_RIGHT:
		printf("KEY_RIGHT\n");
		s->cons->y++;
	break;
	case KEY_DOWN:
		printf("KEY_DOWN\n");
		s->cons->y++;
	break;
	}
}

static int sc_e0 = 0;
static int sc_mk = 0;
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
	case KEY_CAPSLOCK:
		key_caps = ~key_caps;
	break;
	case KEY_TAB:
		putc(BOS_CHAR_TAB);
		s->cons->x+=8;
	break;
	case 0xE0:
		sc_e0 = 1;
	break;
	default:
		sc_mk = !IS_BREAK_CODE(c);
		if (sc_e0) {
			if (sc_mk)
				control_key_handling(s, c);
			sc_e0 = 0;
		} else if (c < MAX_KEYTB_SIZE) {
			int ch = 0;
			if ((key_shift & KB_SHIFT_L)
			     || (key_shift & KB_SHIFT_R))
				ch = keytable_shift[c];
			else
				ch = keytable[c];

			if (key_caps) {
				if (ch >= 'a' || ch <='z') {
					ch = ch -'a' + 'A';
				} else if (ch >= 'A' || ch <='Z') {
					ch = ch -'A' + 'a';
				}
			}

			if (c == KEY_BS) { /* process backspace key */
				if (s->buflen > 0) {
					s->buf[--s->buflen] = '\0';
					putc('\b');
				}
			} else if (c == KEY_ENTER) {
				s->buf[s->buflen] = 0;
				s->prevbuflen = s->buflen;
				strcpy(s->prevbuf, s->buf);
				command_exec(s);
				s->buflen = 0;
				s->buf[0] = 0;
			} else if (ch != 0) {
				putc(ch);
				if (s->buflen < MAX_CMD_BUF_SIZE - 1) {
					s->buf[s->buflen++] = ch;
				}
			} else {
				printf_scancode(s, c);
			}
		} else {
			// printf_scancode(s, c);
		}
	}

	return 0;
}

