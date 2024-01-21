// #include <assert.h>
// #include <stdio.h>
// #include <stdlib.h>
// #include <string.h>
// #include <time.h>
//
// #include "malloc.h"
//
// #define N 1
//
// void benchmark(const char *filename) {
//     FILE *file = fopen(filename, "w+");
//
//     for (int n = 1; n < 100; n += 100) {
//         char *arrays[N];
//
//         clock_t start = clock();
//
//         for (int i = 0; i < N; ++i) {
//             arrays[i] = malloc(sizeof(char) * n);
//
//             assert(arrays[i] != NULL && "Malloc failed to allocate memory (NULL returned)");
//
//             memset(arrays[i], 77, sizeof(char) * n);
//         }
//
//         for (int i = 0; i < N; ++i) {
//             free(arrays[i]);
//         }
//
//         clock_t end = clock();
//
//         fprintf(file, "%d %d %lf\n", N, n, difftime(end, start) / CLOCKS_PER_SEC);
//     }
//
//     // show_alloc_mem_ex();
// }
//
// int main(int argc, char **argv) {
//     if (argc < 2) {
//         printf("usage: benchmark filename\n");
//
//         return 0;
//     }
//
//     benchmark(argv[1]);
//
//     return (0);
// }

#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "malloc.h"

#define M (1024 * 1024)

void print(char *s) { write(1, s, strlen(s)); }

int main() {
    char *addr1;
    char *addr2;
    char *addr3;

    addr1 = (char *)malloc(16 * M);
    if (addr1 == NULL) {
        print("Failed to allocate memory\n");
        exit(1);
    }
    strcpy(addr1, "Hello world!\n");
    print(addr1);
    addr2 = (char *)malloc(16 * M);
    if (addr2 == NULL) {
        print("Failed to allocate memory\n");
        exit(1);
    }
    addr3 = (char *)realloc(addr1, 128 * M);
    if (addr3 == NULL) {
        print("Failed to reallocate memory\n");
        exit(1);
    }
    addr3[127 * M] = 42;
    print(addr3);
    return (0);
}
