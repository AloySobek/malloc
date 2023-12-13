#include "malloc.h"

void *calloc(size_t nmemb, size_t size) {
    size_t res;

    if (__builtin_mul_overflow(nmemb, size, &res)) {
        return NULL;
    }

    void *ptr = malloc(res);

    if (ptr == NULL) {
        return NULL;
    }

    memset(ptr, 0, res);

    return ptr;
}
