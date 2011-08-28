#ifndef _OS_KEYBOARD_H
#define _OS_KEYBOARD_H
#include "os_fifo.h"
#define KB_CMD		0x64

#define KB_DATA_PORT	0x0060
#define KB_WR_MODE	0x60
#define KBC_MODE	0x47

void init_kb(struct FIFO32 *fifo, int offset);

/* key map */
enum KEY_NUM {
KEY_1=2,
KEY_2,
KEY_3,
KEY_4,
KEY_5,
KEY_6,
KEY_7,
KEY_8,
KEY_9, /* 0x0A */
KEY_0, /* 0x0B */
KEY_q=0x10,
KEY_SHIFT=0x2A, /* 0x2A */
};

/* change keyboard fifo*/
void kb_change_fifo(struct FIFO32 *fifo);

#endif /* _OS_KEYBOARD_H */
