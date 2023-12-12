#ifndef MALLOC_H
#define MALLOC_H

#include <assert.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define TINY_N 1024
#define TINY_SIZE 64

#define SMALL_N 256
#define SMALL_SIZE 1024

#define MAX_HEAPS 8

struct heap;

// 2^5(32) bytes
struct block {
    struct heap *heap;

    struct block *next;
    struct block *prev;

    size_t size;
};

struct heap {
    struct heap *next;
    struct heap *prev;

    struct block *tiny;
    struct block *small;
};

extern struct heap *_heap;

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void free(void *ptr);

struct block *_add_block(struct block *head, struct block *block);
struct block *_remove_block(struct block *head, struct block **block);

void *_allocate_heap(size_t tiny_n, size_t tiny_size, size_t small_n, size_t small_size);

#endif