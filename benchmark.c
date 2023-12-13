#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10000

void benchmark(const char *filename) {
    FILE *file = fopen(filename, "w+");

    for (int n = 1; n < 10000; n += 100) {
        char *arrays[N];

        clock_t start = clock();

        for (int i = 0; i < N; ++i) {
            arrays[i] = malloc(sizeof(char) * n);

            assert(arrays[i] != NULL && "Malloc failed to allocate memory (NULL returned)");

            memset(arrays[i], 77, sizeof(char) * n);
        }

        for (int i = 0; i < N; ++i) {
            free(arrays[i]);
        }

        clock_t end = clock();

        fprintf(file, "%d %d %lf\n", N, n, difftime(end, start) / CLOCKS_PER_SEC);
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("usage: benchmark filename\n");

        return 0;
    }

    benchmark(argv[1]);

    return (0);
}
