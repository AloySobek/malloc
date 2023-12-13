#include "malloc.h"

void *_heap = NULL;

void *_allocate_heap(size_t tiny_n, size_t tiny_size, size_t small_n, size_t small_size) {
    size_t size = sizeof(struct heap) + sizeof(struct block) * tiny_n * small_n +
                  tiny_n * tiny_size + small_n * small_size;

    size += size % getpagesize();

    void *heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    assert(heap != MAP_FAILED);

    memset(heap, 0, sizeof(struct heap));

    void *iter = heap + sizeof(struct heap);

    for (size_t i = 0; i < tiny_n; ++i, iter += sizeof(struct block) + tiny_size) {
        ((struct block *)iter)->heap = heap;

        ((struct heap *)heap)->tiny = _add_block(((struct heap *)heap)->tiny, iter);

        ((struct block *)iter)->from_heap_size = tiny_size;
        ((struct block *)iter)->size = 0;
    }

    iter = heap + sizeof(struct heap) + sizeof(struct block) * tiny_n + tiny_size * tiny_n;

    for (size_t i = 0; i < small_n; ++i, iter += sizeof(struct block) + small_size) {
        ((struct block *)iter)->heap = heap;

        ((struct heap *)heap)->small = _add_block(((struct heap *)heap)->small, iter);

        ((struct block *)iter)->from_heap_size = small_size;
        ((struct block *)iter)->size = 0;
    }

    return heap;
}
