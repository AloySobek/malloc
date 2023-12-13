#include "malloc.h"

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL && size) {
        return malloc(size);
    } else if (ptr != NULL && !size) {
        free(ptr);

        return NULL;
    } else if (ptr != NULL && size) {
        void *new = malloc(size);

        struct block *block = ptr - sizeof(struct block);

        memcpy(new, ptr, size < block->size ? size : block->size);

        return new;
    } else {
        return NULL;
    }
}
