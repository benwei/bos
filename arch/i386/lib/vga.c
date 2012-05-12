#include "i386/inc/vga.h"
#include "os_sys.h"
#include "os_stdio.h"
#include "os_stdio2.h"

void vga_setcursor(int x, int y) {
	int pos = y*MAX_COLS + x;
	asm_cli();
	outb(OS_CRTCR_ADDR, OS_CRTCR_CURSOR_LOC_HIGH);
	outb(OS_CRTCR_DATA, (pos>>8) & 0xFF);
	outb(OS_CRTCR_ADDR, OS_CRTCR_CURSOR_LOC_LOW);
	outb(OS_CRTCR_DATA, pos & 0xFF);
	asm_sti();
}

void vga_scrollto(int pos) {
	asm_cli();
	outb(OS_CRTCR_ADDR, OS_CRTCR_SADDR_HIGH);
	/* 80 * lineup => two bytes,
	  put high bytes to 0xC, put low bytes to 0xD */
	outb(OS_CRTCR_DATA, (pos>>8) & 0xFF);
	outb(OS_CRTCR_ADDR, OS_CRTCR_SADDR_LOW);
	outb(OS_CRTCR_DATA, pos & 0xFF);
	asm_sti();
}
