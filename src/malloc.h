#ifndef MALLOC_H
#define MALLOC_H

#include <stdio.h>

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

#endif
