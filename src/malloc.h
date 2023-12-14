#ifndef MALLOC_H
#define MALLOC_H

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define TINY_N 16384
#define TINY_SIZE 64

#define SMALL_N 2048
#define SMALL_SIZE 16384

#define MAX_HEAPS 4

struct heap;

struct block {
    struct heap *heap;

    struct block *next;
    struct block *prev;

    size_t from_heap_size;
    size_t size;
};

struct heap {
    struct heap *next;

    struct block *tiny;
    struct block *small;
    struct block *large;
};

struct stats {
    size_t max_heap_size;
    size_t peak_heap_size;
    size_t current_heap_size;
    size_t current_heap_usage;
    size_t n_blocks;

    struct block *blocks;
};

extern void *_heap;
extern struct stats _stats;

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);

struct block *_add_block(struct block *head, struct block *block);
struct block *_remove_block(struct block *head, struct block **block);

void *_allocate_heap(size_t tiny_n, size_t tiny_size, size_t small_n, size_t small_size);

#endif
