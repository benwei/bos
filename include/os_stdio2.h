#ifndef __STDIO2_H__
#define __STDIO2_H__
// read input data
unsigned getc(void);

unsigned char getchar(void);

// output data to console
void putc(unsigned char c);

/* value swapping */
#define SWAP8(X) 	(X)
#define SWAP16(X) 	((((X) & 0x00ff) << 8) | \
			 ((X) >> 8))
#define SWAP32(X) 	((((X) & 0x000000ff) << 24) | \
			 (((X) & 0x0000ff00) << 8) | \
			 (((X) & 0x00ff0000) >> 8) | \
			 (((X) & 0xff000000) >> 24))

#define va_list __builtin_va_list
#define va_start(args, fmt) __builtin_va_start(args, fmt)
#define va_arg(args, type) __builtin_va_arg(args, type)
#define va_end(args) __builtin_va_end(args)

void move_cursor(int x, int y);
void putchar(char c);

int printf(const char *fmt, ...);

/* add '\0' at the tail of buf
* @param buf input buffer not need to initialize
* @param fmt format string, reference note
* @return The length without null char is returned;
*/
int sprintf(char *buf, const char *fmt, ...);

/* string.h */

void puts(const char *s);

/* console */
#define clrscr() _clrscr();

/* floppy definitions */
#define CMOS_FD_NA    0
#define CMOS_FD_360kb 1
#define CMOS_FD_1_2mb 2
#define CMOS_FD_720kb 3
#define CMOS_FD_1_44mb 4
char get_floppy_info(void);
const char *get_fd_msg(unsigned char c);

#endif /* __STDIO2_H__ */
