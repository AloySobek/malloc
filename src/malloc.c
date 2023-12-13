#include "malloc.h"

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    if (_heap == NULL) {
        _heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);
    }

    // void *iter = _heap;

    // for (int i = 0; i < MAX_HEAPS; ++i) {
    void *block = NULL;

    if (size <= TINY_SIZE) {
        ((struct heap *)_heap)->tiny =
            _remove_block(((struct heap *)_heap)->tiny, (struct block **)&block);
    } else if (size <= SMALL_SIZE) {
        ((struct heap *)_heap)->small =
            _remove_block(((struct heap *)_heap)->small, (struct block **)&block);
    } else {
        // break;
    }

    if (block != NULL) {
        return block + sizeof(struct block);
    }

    //     if (((struct heap *)iter)->next == NULL) {
    //         struct heap *heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);

    //         ((struct heap *)iter)->next = heap;
    //         heap->prev = iter;
    //     }

    //     iter = ((struct heap *)iter)->next;
    // }

    block = mmap(NULL, sizeof(struct block) + size, PROT_READ | PROT_WRITE,
                 MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (block == MAP_FAILED) {
        return NULL;
    }

    ((struct block *)block)->from_heap_size = 0;
    ((struct block *)block)->size = size;

    return block + sizeof(struct block);
}
