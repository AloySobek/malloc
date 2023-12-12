#include "malloc.h"

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    if (_heap == NULL) {
        _heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);
    }

    struct heap *iter = _heap;

    for (int i = 0; i < MAX_HEAPS; ++i, iter = iter->next) {
        struct block *block = NULL;

        if (size <= TINY_SIZE) {
            iter->tiny = _remove_block(iter->tiny, &block);
        } else if (size <= SMALL_SIZE) {
            iter->small = _remove_block(iter->small, &block);
        } else {
            break;
        }

        if (block != NULL) {
            return (void *)block + sizeof(struct block);
        }

        if (iter->next == NULL) {
            struct heap *heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);

            iter->next = heap;
            heap->prev = iter;
        }
    }

    void *ptr = mmap(NULL, sizeof(struct block) + size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (ptr == MAP_FAILED) {
        return NULL;
    }

    (*((struct block *)ptr)).size = size;

    return ptr + sizeof(struct block);
}
