#ifndef STRING_H
#define STRING_H

typedef int size_t;

void strcpy(char *dest, const char *src);

size_t strlen(const char *s);

unsigned int strspn(const char *s, const char *accept);

char * strpbrk(const char * cs,const char * ct);

unsigned long 
strtoul(const char *str, char **endptr, int requestedbase);

int strcmp(const char *s1, const char *s2);

int strncmp(const char *s1, const char *s2, int n);

void *memcpy(void *dest, const void *src, int cnt);

void *memset(void *dest, int c, unsigned int size);

#endif
