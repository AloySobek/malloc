#include "malloc.h"

void *myrealloc(void *ptr, size_t size) {
    void *new = malloc(size);

    size_t n = ((struct block *)(ptr - sizeof(struct block)))->size;

    if (size < n) {
        n = size;
    }

    memcpy(new, ptr, n);

    free(ptr);

    return new;
}
