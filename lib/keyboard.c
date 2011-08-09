#include "os_sys.h"
#include "os_stdio.h"
#include "os_keyboard.h"
#include "os_fifo.h"

static struct FIFO32 *keyfifo;
static int key_offset;
/* int 21 (irq 1) for PS/2 keyboard */
void _inthdr21(int *esp)
{
	unsigned char code;
	/* ack PIC0 interrupt handled */
	outb(PIC0_OCW2, 0x61);
	code = inb(KB_DATA_PORT);
	fifo32_put(keyfifo, code+key_offset);
	return ;
}

void init_kb(struct FIFO32 *fifo, int offset)
{
	key_offset = offset;
	keyfifo = fifo;
	_wait_input_empty();
	outb(KB_CMD, KB_WR_MODE);
	_wait_input_empty();
	outb(KB_DATA_PORT, KBC_MODE);
}

void kb_change_fifo(struct FIFO32 *fifo)
{
	asm_cli();
	keyfifo = fifo;
	asm_sti();
	return ;
}

#define KB_STA 0x0064
#define KB_SEND_NOTREADY 0x02

/* io port function */

unsigned char read_input_port(void)
{
	_wait_input_empty();
	outb(KB_CMD, 0xC0);
	_wait_output_full();	
	return inb(0x60);
}

unsigned char read_output_port(void)
{
	_wait_input_empty();
	outb(KB_CMD, 0xD0);
	_wait_output_full();	
	return inb(0x60);
}

void write_input_port(unsigned char c)
{
	_wait_input_empty();
	outb(KB_CMD, 0xD1);
	_wait_output_full();	
	outb(0x60, c);
}

void write_output_port(unsigned char c)
{
	_wait_input_empty();
	outb(KB_CMD, 0xD2);
	_wait_output_full();	
	outb(0x60, c);
}

void kb_enable(int enabled)
{
	_wait_input_empty();
	if (enabled)
		outb(KB_CMD, 0xAE);
	else
		outb(KB_CMD, 0xAD);	
}

void _wait_input_empty(void)
{
	char __b=0;
	do {
		__b = inb(KB_STA);
	} while (__b & KB_SEND_NOTREADY);
}

void _wait_output_full(void)
{
	char __b=0;
	do {
		__b = inb(KB_STA);
	} while (!(__b & 0x01));
}

