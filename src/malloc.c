#include "malloc.h"

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    pthread_mutex_lock(&_mutex);
    struct block *block = _get_block(size);
    pthread_mutex_unlock(&_mutex);

    return block ? block + 1 : NULL;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    pthread_mutex_lock(&_mutex);
    _return_block(block);
    pthread_mutex_unlock(&_mutex);
}

void *realloc(void *ptr, size_t size) {
    if (ptr == NULL && size) {
        return malloc(size);
    } else if (ptr != NULL && !size) {
        free(ptr);

        return NULL;
    } else if (ptr != NULL && size) {
        void *new = malloc(size);

        if (!new) {
            return NULL;
        }

        pthread_mutex_lock(&_mutex);
        struct block *block = ptr - sizeof(struct block);

        size = size < block->size ? size : block->size;

        for (size_t i = 0; i < size; ++i) {
            *(char *)(new + i) = *(char *)(ptr + i);
        }
        pthread_mutex_unlock(&_mutex);

        return new;
    } else {
        return NULL;
    }
}

void *calloc(size_t nmemb, size_t size) {
    size_t res;

    if (__builtin_mul_overflow(nmemb, size, &res)) {
        return NULL;
    }

    void *ptr = malloc(res);

    if (ptr == NULL) {
        return NULL;
    }

    pthread_mutex_lock(&_mutex);
    for (size_t i = 0; i < res; ++i) {
        *(char *)(ptr + i) = 0;
    }
    pthread_mutex_unlock(&_mutex);

    return ptr;
}
