#include "malloc.h"

void free(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    if (block->size <= TINY_SIZE) {
        block->heap->tiny = _add_block(block->heap->tiny, block);
    } else if (block->size <= SMALL_SIZE) {
        block->heap->small = _add_block(block->heap->small, block);
    } else {
        munmap(block, block->size + sizeof(struct block));
    }
}
