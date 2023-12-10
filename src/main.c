#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#define TINY_N 32
#define TINY_SIZE 128

#define SMALL_N 64
#define SMALL_SIZE 1024

#define LARGE_N 32
#define LARGE_SIZE 16384

struct meta {
    int free;
    int length;
};

struct block {
    int free;
    int length;

    struct block *next;
    struct block *prev;
};

struct heap {
    void *tiny;
    void *small;
    void *large;

    void *extra;
};

void *_heap = NULL;

void *_global_ptr;

struct block *_add_block(struct block *head, struct block *block) {
    if (block == NULL) {
        return NULL;
    }

    if (head == NULL) {
        block->next = block;
        block->prev = block;

        return block;
    } else {
        block->prev = head->prev;
        block->next = head;

        head->prev->next = block;
        head->prev = block;

        return head;
    }
}

struct block *_remove_block(struct block *head) {}

void *_allocate_heap() {
    size_t size = 0;

    size += TINY_N * TINY_SIZE + SMALL_N * SMALL_SIZE + LARGE_N * LARGE_SIZE;
    size += sizeof(struct heap);
    size += (sizeof(struct meta) * TINY_N * SMALL_N * LARGE_N);
    size += size % getpagesize();

    _heap = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    assert(_heap != MAP_FAILED && "Heap allocation failed");

    ((struct heap *)_heap)->tiny = _heap + sizeof(struct heap);
    ((struct heap *)_heap)->small =
        ((struct heap *)_heap)->tiny + ((sizeof(struct meta) * TINY_N) + (TINY_N * TINY_SIZE));
    ((struct heap *)_heap)->large =
        ((struct heap *)_heap)->small + ((sizeof(struct meta) * SMALL_N) + (SMALL_N * SMALL_SIZE));

    void *iter = ((struct heap *)_heap)->tiny;

    for (int i = 0; i < TINY_N; ++i, iter += (sizeof(struct meta) + TINY_SIZE)) {
        (*((struct block *)iter)).free = 1;
        (*((struct block *)iter)).length = TINY_SIZE;

        _add_free_block(((struct heap *)_heap)->tiny, iter);
    }

    iter = ((struct heap *)_heap)->small;

    for (int i = 0; i < SMALL_N; ++i, iter += (sizeof(struct meta) + SMALL_SIZE)) {
        (*((struct meta *)iter)).free = 1;

        (*((struct meta *)iter)).size = SMALL;
    }

    iter = large;

    for (int i = 0; i < LARGE_N; ++i, iter += (sizeof(struct meta) + LARGE_SIZE)) {
        (*((struct meta *)iter)).free = 1;

        (*((struct meta *)iter)).size = LARGE;
    }

    _global_ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    assert(_global_ptr != MAP_FAILED);

    void *tiny = _global_ptr;
    void *small = tiny + ((sizeof(struct meta) * TINY_N) + (TINY_N * TINY_SIZE));
    void *large = small + ((sizeof(struct meta) * SMALL_N) + (SMALL_N * SMALL_SIZE));

    void *iter = tiny;

    for (int i = 0; i < TINY_N; ++i, iter += (sizeof(struct meta) + TINY_SIZE)) {
        (*((struct meta *)iter)).free = 1;

        (*((struct meta *)iter)).size = TINY;
    }

    iter = small;

    for (int i = 0; i < SMALL_N; ++i, iter += (sizeof(struct meta) + SMALL_SIZE)) {
        (*((struct meta *)iter)).free = 1;

        (*((struct meta *)iter)).size = SMALL;
    }

    iter = large;

    for (int i = 0; i < LARGE_N; ++i, iter += (sizeof(struct meta) + LARGE_SIZE)) {
        (*((struct meta *)iter)).free = 1;

        (*((struct meta *)iter)).size = LARGE;
    }
}

void *mymalloc(size_t size) {
    if (size < TINY_SIZE) {
        void *iter = _global_ptr;

        while ((*((struct meta *)iter)).size == TINY) {
            if ((*((struct meta *)iter)).free) {
                (*((struct meta *)iter)).free = 0;

                return iter + sizeof(struct meta);
            }

            iter += (sizeof(struct meta) + TINY_SIZE);
        }

        return NULL;
    } else if (size < SMALL_SIZE) {
        void *iter = _global_ptr + ((sizeof(struct meta) * TINY_N) + (TINY_N * TINY_SIZE));

        while ((*((struct meta *)iter)).size == SMALL) {
            if ((*((struct meta *)iter)).free) {
                (*((struct meta *)iter)).free = 0;

                return iter + sizeof(struct meta);
            }

            iter += (sizeof(struct meta) + SMALL_SIZE);
        }

        return NULL;
    } else if (size < LARGE_SIZE) {
        void *iter = _global_ptr + ((sizeof(struct meta) * TINY_N) + (TINY_N * TINY_SIZE)) +
                     ((sizeof(struct meta) * SMALL_N) + (SMALL_N * SMALL_SIZE));

        while ((*((struct meta *)iter)).size == LARGE) {
            if ((*((struct meta *)iter)).free) {
                (*((struct meta *)iter)).free = 0;

                return iter + sizeof(struct meta);
            }

            iter += (sizeof(struct meta) + LARGE_SIZE);
        }

        return NULL;
    } else {
        void *ptr = mmap(NULL, sizeof(struct meta) + size, PROT_READ | PROT_WRITE,
                         MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

        (*((struct meta *)ptr)).length = size;
        (*((struct meta *)ptr)).size = CUSTOM;

        return ptr + sizeof(struct meta);
    }
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    if ((*((struct meta *)(ptr - sizeof(struct meta)))).size == CUSTOM) {
        munmap(ptr - sizeof(struct meta),
               (*((struct meta *)(ptr - sizeof(struct meta)))).size + sizeof(struct meta));
    } else {
        (*((struct meta *)(ptr - sizeof(struct meta)))).free = 1;
    }
}

void *myrealloc(void *ptr, size_t size) {
    myfree(ptr);

    return mymalloc(size);
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
        char *new_array = malloc(sizeof(char) * rand() % (10000 + 1));

        assert(array != NULL);

        free(array);
    }

    end = clock();

    double diff = difftime(end, start) / CLOCKS_PER_SEC;

    return mydiff / diff;
}

int main(int argc, char **argv) {
    _prealloc();

    double average = 0.0f;

    for (int i = 0; i < 1000; ++i) {
        average += benchmark();
    }

    printf("On average my malloc is %.8lf times slower than standard malloc\n", average / 1000.0f);

    return (0);
}
