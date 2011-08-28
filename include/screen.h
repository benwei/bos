#ifndef SCREEN_H
#define SCREEN_H

#define OS_VIDEO_ADDR    0xB8000
#define OS_VIDEO_MEMSIZE 0x8000
#define MAX_COLS 80
#define MAX_ROWS 25

extern void clearline();

#endif /* SCREEN_H */
