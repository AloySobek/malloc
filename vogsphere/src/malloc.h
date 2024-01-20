#ifndef MALLOC_H
#define MALLOC_H

#include <pthread.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>

#define _INSERT_BLOCK(head, block)                                                                 \
    (struct block *)_insert_node((struct doubly_linked_list_node *)head,                           \
                                 (struct doubly_linked_list_node *)block)

#define _REMOVE_BLOCK(head, block)                                                                 \
    (struct block *)_remove_node((struct doubly_linked_list_node *)head,                           \
                                 (struct doubly_linked_list_node **)&block)

struct cluster;
struct heap;

enum type { TinyBlock, SmallBlock, LargeBlock, MaxBlock };

struct doubly_linked_list_node {
    struct doubly_linked_list_node *next;
    struct doubly_linked_list_node *prev;
};

struct block {
    struct doubly_linked_list_node node;

    struct cluster *cluster;

    size_t size;

    enum type type;
};

struct cluster {
    struct doubly_linked_list_node node;

    struct block *available_blocks;
    struct block *occupied_blocks;

    size_t capacity;
    size_t size;
};

struct heap {
    struct cluster *available_clusters;
    struct cluster *occupied_clusters;

    size_t capacity;
    size_t size;
};

struct record {
    enum type block_type;

    size_t size;
};

struct config {
    size_t tiny_n;
    size_t tiny_size;
    size_t small_n;
    size_t small_size;
    size_t max_clusters;
    size_t min_available_clusters;
    size_t max_records;
    size_t read_from_env;
};

struct pool {
    struct config cfg;

    struct heap tiny;
    struct heap small;

    struct block *large;

    struct record *records;

    long long n;
};

extern struct pool _pool;
extern pthread_mutex_t _mutex;

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);
void show_alloc_mem();
void show_alloc_mem_ex();

struct block *_get_block(size_t size);
void _return_block(struct block *block);

#endif
