#ifndef __UTIL_H__
#define __UTIL_H__

#include "libio.h"
// system call
char inb(int port);
char outb(int port, char data);
void _syshalt(void);

// keyboard
void kb_enable(int enabled);
void _wait_output_full(void);
void _wait_input_empty(void);

unsigned char read_input_port(void);
unsigned char read_output_port(void);
void write_input_port(unsigned char c);
void write_output_port(unsigned char c);

#define kbwait() _wait_input_empty()

// screen output
void _cputs(const char *msg);
void _clrscr(void);
void _movcursor(char x, char y);


#endif /* __UTIL_H__ */
