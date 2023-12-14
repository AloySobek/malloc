#include "malloc.h"

void *_heap = NULL;
struct stats _stats = {0};

void *_allocate_heap(size_t tiny_n, size_t tiny_size, size_t small_n, size_t small_size) {
    size_t size = ((sizeof(struct heap) + (sizeof(struct block) * tiny_n * small_n) +
                    (tiny_n * tiny_size) + (small_n * small_size)) +
                   getpagesize() - 1) &
                  ~(getpagesize() - 1);

    void *heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (heap == MAP_FAILED) {
        return NULL;
    }

    ((struct heap *)heap)->next = NULL;
    ((struct heap *)heap)->tiny = NULL;
    ((struct heap *)heap)->small = NULL;

    void *iter = heap + sizeof(struct heap);

    for (size_t i = 0; i < tiny_n; ++i, iter += sizeof(struct block) + tiny_size) {
        ((struct block *)iter)->heap = heap;

        ((struct heap *)heap)->tiny = _add_block(((struct heap *)heap)->tiny, iter);

        ((struct block *)iter)->from_heap_size = tiny_size;
        ((struct block *)iter)->size = 0;
    }

    iter = heap + sizeof(struct heap) + (sizeof(struct block) * tiny_n) + (tiny_n * tiny_size);

    for (size_t i = 0; i < small_n; ++i, iter += sizeof(struct block) + small_size) {
        ((struct block *)iter)->heap = heap;

        ((struct heap *)heap)->small = _add_block(((struct heap *)heap)->small, iter);

        ((struct block *)iter)->from_heap_size = small_size;
        ((struct block *)iter)->size = 0;
    }

    return heap;
}
