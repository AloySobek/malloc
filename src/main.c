#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define TINY_N 128
#define TINY_SIZE 32

#define SMALL_N 128
#define SMALL_SIZE 64

#define LARGE_N 128
#define LARGE_SIZE 256

struct meta {
    size_t size;
};

void *mymalloc(size_t size) {
    void *ptr = mmap(NULL, sizeof(struct meta) + size, PROT_READ | PROT_WRITE,
                     MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (ptr == MAP_FAILED) {
        return NULL;
    }

    (*((struct meta *)ptr)).size = size;

    return ptr + sizeof(struct meta);
}

void myfree(void *ptr) {
    if (ptr == NULL) {
        return;
    }

    munmap(ptr, (*((struct meta *)(ptr - sizeof(struct meta)))).size);
}

void *myrealloc(void *ptr, size_t size) {
    myfree(ptr);

    return mymalloc(size);
}

int test() {
    int *array = NULL;

    array = mymalloc(sizeof(int) * 100);

    for (int i = 0; i < 100; ++i) {
        array[i] = i;
    }

    for (int i = 0; i < 100; ++i) {
        printf("%d ", array[i]);
    }

    printf("\n");

    myfree(array);

    return 0;
}

int main(int argc, char **argv) {
    assert(test() == 0);

    return (0);
}
