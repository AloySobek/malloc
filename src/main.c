#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <unistd.h>

#define N 5

typedef struct __s_malloc_state {
    int first_call; // Lazy loading bit, dirty bit, etc...
} __t_malloc_state;

__t_malloc_state __malloc_state;

size_t global_variable = 0;

void prealloc() {}

void *mymalloc(size_t size) {
    void *ptr;

    ptr = mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (ptr == MAP_FAILED) {
        printf("Mapping Failed\n");

        return (NULL);
    }

    global_variable = size;

    return ptr;
}

void myfree(void *ptr) {
    int err = munmap(ptr, global_variable);

    if (err != 0) {
        printf("UnMapping Failed\n");
    }
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

    myfree(array);

    // if (ptr == MAP_FAILED) {
    //     printf("Mapping Failed\n");

    //     return 1;
    // }

    // int size = getpagesize() - 100;

    // // Fill the elements of the array
    // for (int i = 0; i < size; i++) {
    //     ptr[i] = i * 10;
    // }

    // // Print the elements of the array
    // printf("The elements of the array => ");

    // for (int i = 0; i < N; i++)
    //     printf("[%d] ", ptr[i]);

    // printf("\n");

    // int err = munmap(ptr, 10 * sizeof(int));

    // if (err != 0) {
    //     printf("UnMapping Failed\n");

    //     return 1;
    // }
    //
    return (0);
}

int main(int argc, char **argv) {
    assert(test() == 0);

    return (0);
}
