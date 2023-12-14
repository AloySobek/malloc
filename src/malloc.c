#include "malloc.h"

void *malloc(size_t size) {
    if (!_heap) {
        _heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);
    }

    if (size == 0) {
        return NULL;
    }

    void *block = NULL;

    if (_heap) {
        void *iter = _heap;

        for (int i = 0; i < MAX_HEAPS && !block; ++i) {
            if (size <= TINY_SIZE) {
                ((struct heap *)_heap)->tiny =
                    _remove_block(((struct heap *)_heap)->tiny, (struct block **)&block);
            } else if (size <= SMALL_SIZE) {
                ((struct heap *)_heap)->small =
                    _remove_block(((struct heap *)_heap)->small, (struct block **)&block);
            }

            if (!((struct heap *)iter)->next) {
                ((struct heap *)iter)->next =
                    _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);

                if (!((struct heap *)iter)->next) {
                    break;
                }
            }

            iter = ((struct heap *)iter)->next;
        }
    }

    if (!block) {
        if (_stats.current_heap_size - _stats.current_heap_usage >= size) {
            for (struct block *iter = _stats.blocks; iter; iter = iter->next) {
                // Return block if found
            }
            // We have enough free bytes by the memory is too fragmented to allocate continuum block
            // of memory
        } else {
            // Heap is not large enough - new block required
        }

        if ((block = mmap(
                 NULL, ((sizeof(struct block) + size) + getpagesize() - 1) & ~(getpagesize() - 1),
                 PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)) == MAP_FAILED) {
            block = NULL;
        }
    }

    if (block) {
        ((struct block *)block)->size = size;

        return block + sizeof(struct block);
    }

    return NULL;
}
