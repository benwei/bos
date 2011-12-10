#ifndef STRING_H
#define STRING_H

#include "types.h"

void strcpy(char *dest, const char *src);

size_t strlen(const char *s);

unsigned int strspn(const char *s, const char *accept);

char * strpbrk(const char * cs,const char * ct);

unsigned long 
strtoul(const char *str, char **endptr, int requestedbase);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, int n);

void *memcpy(void *dest, const void *src, int n);

void *memmove(void *dest, const void *src, size_t n);

void *memset(void *dest, int c, unsigned int size);

void panic(const char *fmt, ...);
#endif
