#include "malloc.h"

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    if (block->from_heap_size == TINY_SIZE) {
        block->heap->tiny = _add_block(block->heap->tiny, block);
    } else if (block->from_heap_size == SMALL_SIZE) {
        block->heap->small = _add_block(block->heap->small, block);
    } else {
        munmap(block, sizeof(struct block) + block->size);
    }
}
