#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define TINY_N 1024
#define TINY_SIZE 64

#define SMALL_N 128
#define SMALL_SIZE 16384

// 2^5(32) bytes
struct block {
    struct block *next;
    struct block *prev;

    size_t size;
};

struct heap {
    struct block *tiny;
    struct block *small;

    size_t size;
};

void *_heap = NULL;

struct block *_add_block(struct block *head, struct block *block) {
    if (block == NULL) {
        return head;
    }

    if (head == NULL) {
        block->next = block;
        block->prev = block;

        return block;
    } else {
        block->next = head;
        block->prev = head->prev;

        block->prev->next = block;
        block->next->prev = block;

        return head;
    }
}

struct block *_remove_block(struct block *head, struct block **block) {
    if (head == NULL) {
        return NULL;
    }

    *block = head->prev;

    (*block)->prev->next = (*block)->next;
    (*block)->next->prev = (*block)->prev;

    (*block)->prev = NULL;
    (*block)->next = NULL;

    if (head == *block) {
        return NULL;
    }

    return head;
}

void *_allocate_heap(size_t tiny_n, size_t tiny_size, size_t small_n, size_t small_size) {
    size_t size = sizeof(struct heap) + sizeof(struct block) * tiny_n * small_n +
                  tiny_n * tiny_size + small_n * small_size;

    size += size % getpagesize();

    _heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    assert(_heap != MAP_FAILED);

    ((struct heap *)_heap)->tiny = NULL;
    ((struct heap *)_heap)->small = NULL;
    ((struct heap *)_heap)->size = size;

    void *iter = _heap + sizeof(struct heap);

    for (int i = 0; i < tiny_n; ++i, iter += sizeof(struct block) + tiny_size) {
        ((struct heap *)_heap)->tiny = _add_block(((struct heap *)_heap)->tiny, iter);

        ((struct block *)iter)->size = tiny_size;
    }

    iter = _heap + sizeof(struct heap) + sizeof(struct block) * tiny_n + tiny_size * tiny_n;

    for (int i = 0; i < small_n; ++i, iter += sizeof(struct block) + small_size) {
        ((struct heap *)_heap)->small = _add_block(((struct heap *)_heap)->small, iter);

        ((struct block *)iter)->size = small_size;
    }

    return _heap;
}

void *mymalloc(size_t size) {
    if (_heap == NULL) {
        _heap = _allocate_heap(TINY_N, TINY_SIZE, SMALL_N, SMALL_SIZE);
    }

    if (size <= TINY_SIZE) {
        struct block *block = NULL;

        ((struct heap *)_heap)->tiny = _remove_block(((struct heap *)_heap)->tiny, &block);

        if (block != NULL) {
            return (void *)block + sizeof(struct block);
        }

        return NULL;
    } else if (size <= SMALL_SIZE) {
        struct block *block = NULL;

        ((struct heap *)_heap)->small = _remove_block(((struct heap *)_heap)->small, &block);

        if (block != NULL) {
            return (void *)block + sizeof(struct block);
        }

        return NULL;
    } else {
        void *ptr = mmap(NULL, sizeof(struct block) + size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

        (*((struct block *)ptr)).size = size;

        return ptr + sizeof(struct block);
    }
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    struct block *block = ptr - sizeof(struct block);

    size_t size = ((struct block *)(ptr - sizeof(struct block)))->size;

    if (size <= TINY_SIZE) {
        ((struct heap *)_heap)->tiny =
            _add_block(((struct heap *)_heap)->tiny, ptr - sizeof(struct block));
    } else if (size <= SMALL_SIZE) {
        ((struct heap *)_heap)->small =
            _add_block(((struct heap *)_heap)->small, ptr - sizeof(struct block));
    } else {
        munmap(ptr - sizeof(struct block),
               ((struct block *)(ptr - sizeof(struct block)))->size + sizeof(struct block));
    }
}

void *myrealloc(void *ptr, size_t size) {
    void *new = mymalloc(size);

    memcpy(new, ptr, ((struct block *)(ptr - sizeof(struct block)))->size);

    myfree(ptr);

    return new;
}

double benchmark() {
    srand(time(NULL));

    int times = rand() % 10000 + 1;

    clock_t start = clock();

    for (int i = 0; i < times; ++i) {
        char *array = mymalloc(sizeof(char) * rand() % (10000 + 1));

        assert(array != NULL);

        myfree(array);
    }

    clock_t end = clock();

    double mydiff = difftime(end, start) / CLOCKS_PER_SEC;

    start = clock();

    for (int i = 0; i < times; ++i) {
        char *array = malloc(sizeof(char) * rand() % (10000 + 1));

        assert(array != NULL);

        free(array);
    }

    end = clock();

    double diff = difftime(end, start) / CLOCKS_PER_SEC;

    return mydiff / diff;
}

int main(int argc, char **argv) {
    double average = 0.0f;

    for (int i = 0; i < 1000; ++i) {
        average += benchmark();
    }

    printf("On average my malloc is %.8lf times slower than standard malloc\n", average / 1000.0f);

    return (0);
}
