#include "malloc.h"

void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    if (block->size <= TINY_SIZE) {
        struct heap *heap = block->heap;

        heap->tiny = _add_block(heap->tiny, block);
    } else if (block->size <= SMALL_SIZE) {
        struct heap *heap = block->heap;

        heap->small = _add_block(heap->small, block);
    } else {
        munmap(block, sizeof(struct block) + block->size);
    }
}
