#ifndef __MALLOC_H__
#define __MALLOC_H__
#include <types.h>

void * malloc(size_t size);
void * realloc(void * ptr, size_t size);
void free(void * ptr);

#endif /* __MALLOC_H__ */
