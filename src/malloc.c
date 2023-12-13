#include "malloc.h"

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    if (_heap == NULL) {
        _heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);
    }

    void *iter = _heap;

    for (int i = 0; i < MAX_HEAPS; ++i) {
        void *block = NULL;

        if (size <= TINY_SIZE) {
            ((struct heap *)iter)->tiny =
                _remove_block(((struct heap *)iter)->tiny, (struct block **)&block);
        } else if (size <= SMALL_SIZE) {
            ((struct heap *)iter)->small =
                _remove_block(((struct heap *)iter)->small, (struct block **)&block);
        } else {
            break;
        }

        if (block != NULL) {
            return block + sizeof(struct block);
        }

        if (((struct heap *)iter)->next == NULL) {
            struct heap *heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);

            ((struct heap *)iter)->next = heap;
            heap->prev = iter;
        }
    }

    void *block = mmap(NULL, sizeof(struct block) + size, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (block == MAP_FAILED) {
        return NULL;
    }

    ((struct block *)block)->size = size;

    return block + sizeof(struct block);
}
