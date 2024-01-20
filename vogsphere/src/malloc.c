#include "malloc.h"

static int from_env_or_default(const char *env_name, const size_t dflt) {
    const char *value = getenv(env_name);

    if (value) {
        size_t int_of_string = atoi(value);

        if (int_of_string > 0) {
            return int_of_string;
        }
    }

    return dflt;
}

static void record_alloc(enum type type, size_t size) {
    _pool.records[_pool.n % _pool.cfg.max_records].block_type = type;
    _pool.records[_pool.n++ % _pool.cfg.max_records].size = size;
}

void *malloc(size_t size) {
    if (size == 0) {
        return NULL;
    }

    pthread_mutex_lock(&_mutex);

    if (!_pool.cfg.read_from_env) {
        _pool.cfg.tiny_n = from_env_or_default("MALLOC_TINY_N", _pool.cfg.tiny_n);
        _pool.cfg.tiny_size = from_env_or_default("MALLOC_TINY_SIZE", _pool.cfg.tiny_size);
        _pool.cfg.small_n = from_env_or_default("MALLOC_SMALL_N", _pool.cfg.small_n);
        _pool.cfg.small_size = from_env_or_default("MALLOC_SMALL_SIZE", _pool.cfg.small_size);
        _pool.cfg.max_clusters = from_env_or_default("MALLOC_MAX_CLUSTERS", _pool.cfg.max_clusters);
        _pool.cfg.min_available_clusters =
            from_env_or_default("MALLOC_MIN_AVAILABLE_CLUSTERS", _pool.cfg.min_available_clusters);
        _pool.cfg.max_records = from_env_or_default("MALLOC_MAX_RECORDS", _pool.cfg.max_records);
        _pool.cfg.read_from_env = 1;
    }

    if (_pool.records == NULL) {
        _pool.records = (void *)(_get_block(sizeof(struct record) * _pool.cfg.max_records) + 1);
    }

    struct block *block = _get_block(size);

    record_alloc(block ? block->type : MaxBlock, size);

    pthread_mutex_unlock(&_mutex);

    return block ? block + 1 : NULL;
}

void free(void *ptr) {
    if (!ptr) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    pthread_mutex_lock(&_mutex);

    block->size = 0;

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
        struct block *block = ptr - sizeof(struct block);

        switch (block->type) {
        case TinyBlock: {
            if (size <= _pool.cfg.tiny_size) {
                return ptr;
            }

            break;
        }
        case SmallBlock: {
            if (size <= _pool.cfg.small_size) {
                return ptr;
            }

            break;
        }
        case LargeBlock: {
            if (size <= block->size) {
                return ptr;
            }
            break;
        }
        default: {
            break;
        }
        }

        void *new = malloc(size);

        if (!new) {
            return NULL;
        }

        for (size_t i = 0; i < size; ++i) {
            *(char *)(new + i) = *(char *)(ptr + i);
        }

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

    for (size_t i = 0; i < res; ++i) {
        *(char *)(ptr + i) = 0;
    }

    return ptr;
}
