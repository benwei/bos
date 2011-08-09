#include "os_sys.h"
#include "os_stdio.h"
#include "os_keyboard.h"
#include "os_fifo.h"

/* Initialize the programable interrupt conotroller */
void init_pic(void)
{
	outb(PIC0_IMR,  0xff  );
	outb(PIC1_IMR,  0xff  ); 

	outb(PIC0_ICW1, 0x11  ); 
	outb(PIC0_ICW2, 0x20  ); /* IRQ0-7 */
	outb(PIC0_ICW3, 1 << 2); /* PIC1 IRQ2*/
	outb(PIC0_ICW4, 0x01  ); /**/

	outb(PIC1_ICW1, 0x11  ); 
	outb(PIC1_ICW2, 0x28  ); /* IRQ8-15 INT28-2f*/
	outb(PIC1_ICW3, 2     ); /* */
	outb(PIC1_ICW4, 0x01  ); /* */

	outb(PIC0_IMR,  0xfb  ); /* 11111011 PIC1 */
	outb(PIC1_IMR,  0xff  ); /* 11111111 */
	return;
}

void _inthdr2c(int *esp)
{
	/* Int 2c (irq 12) hanlder for Mouse PS/2 */
	inb(KB_DATA_PORT);
	/* ignore the mouse data */
	/* ack PCI1 & PIC0 interrupt handled */
	outb(PIC1_OCW2, 0x64); /* IRQ-12 PIC1 */
	outb(PIC0_OCW2, 0x62); /* IRQ-02 PIC0 */
	return;
}

void _inthdr27(int *esp)
{
	/* PIC0 Athlon64X2 */
	outb(PIC0_OCW2, 0x67); /* IRQ-07 */
	return;
}

