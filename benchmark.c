#include <assert.h>
#include <check.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define N 10000

// START_TEST(test) { ck_assert_int_eq(5, 5); }
// END_TEST

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

// Suite *test_suite(void) {
//     Suite *s;
//     TCase *tc_core;
//
//     s = suite_create("test");
//
//     /* Core test case */
//     tc_core = tcase_create("Core");
//
//     tcase_add_test(tc_core, test);
//     suite_add_tcase(s, tc_core);
//
//     return s;
// }
//
// int main(void) {
//     int number_failed;
//     Suite *s;
//     SRunner *sr;
//     s = test_suite();
//     sr = srunner_create(s);
//     srunner_run_all(sr, CK_NORMAL);
//     number_failed = srunner_ntests_failed(sr);
//     srunner_free(sr);
//     return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
// }
