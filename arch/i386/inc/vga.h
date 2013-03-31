#ifndef VGA_H
#define VGA_H

#define OS_VIDEO_ADDR     0xB8000
#define OS_VIDEO_MEMSIZE  0x8000

/* used outb for CRT controller */
#define OS_CRTCR_ADDR       0x3D4
#define OS_CRTCR_DATA       0x3D5
#define OS_CRTCR_SADDR_HIGH 0x0C
#define OS_CRTCR_SADDR_LOW  0x0D
 /* control cursor location  */
#define OS_CRTCR_CURSOR_LOC_HIGH 0x0E
#define OS_CRTCR_CURSOR_LOC_LOW  0x0F

#define MAX_COLS 80
#define MAX_ROWS 25

extern void vga_setcursor(int x, int y);
extern void vga_scrollto(int pos);

#endif /* VGA_H */
