#ifndef MALLOC_H
#define MALLOC_H

#include <pthread.h>
#include <sys/mman.h>
#include <sys/resource.h>
#include <unistd.h>

#define TINY_N 16384
#define TINY_SIZE 64

#define SMALL_N 1024
#define SMALL_SIZE 16384

#define MAX_CLUSTERS 32

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

struct pool {
    struct heap tiny;
    struct heap small;

    struct block *large;
};

extern struct pool _pool;
extern pthread_mutex_t _mutex;

void *malloc(size_t size);
void *realloc(void *ptr, size_t size);
void *calloc(size_t nmemb, size_t size);
void free(void *ptr);

struct block *_get_block(size_t size);
void _return_block(struct block *block);

#endif
