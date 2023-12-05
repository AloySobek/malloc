#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <time.h>
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

void benchmark(int times, int bytes) {
    clock_t start = clock();

    for (int i = 0; i < times; ++i) {
        char *array = mymalloc(sizeof(char) * bytes);

        myfree(array);
    }

    clock_t end = clock();

    double mydiff = difftime(end, start) / CLOCKS_PER_SEC;

    printf("My malloc took %.8lf seconds to allocate and deallocate %d bytes %d times\n", mydiff,
           bytes, times);

    start = clock();

    for (int i = 0; i < times; ++i) {
        char *array = malloc(sizeof(char) * bytes);

        free(array);
    }

    end = clock();

    double diff = difftime(end, start) / CLOCKS_PER_SEC;

    printf("Standard malloc took %.8lf seconds to allocate and deallocate %d bytes %d times\n",
           diff, bytes, times);

    printf("My malloc is %.8lf times slower than standard malloc\n", mydiff / diff);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        printf("usage: malloc times bytes\n");

        return 0;
    }

    int times = atoi(argv[1]);
    int bytes = atoi(argv[2]);

    benchmark(times, bytes);

    return (0);
}
