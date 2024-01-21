#include "malloc.h"
#include <assert.h>
#include <check.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void *thread_function() {
    char *block = _malloc(100);
    ck_assert_ptr_nonnull(block);

    _free(block);
    return NULL;
}

void *thread_function_stress() {
    for (int i = 0; i < 100; i++) {
        int size = rand() % 1000 + 1; // Random size between 1 and 1000
        char *block = _malloc(size);
        ck_assert_ptr_nonnull(block);

        _free(block);
    }
    return NULL;
}

START_TEST(allocate_one_byte_then_free) {
    char *byte = _malloc(1);

    ck_assert_ptr_nonnull(byte);

    _free(byte);
}
END_TEST

START_TEST(allocate_large_block) {
    char *block = _malloc(1000000); // Allocate a large memory block

    ck_assert_ptr_nonnull(block);

    _free(block);
}
END_TEST

START_TEST(allocate_multiple_small_blocks) {
    const int NUM_BLOCKS = 100;
    char *blocks[NUM_BLOCKS];

    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = _malloc(10); // Allocate small blocks
        ck_assert_ptr_nonnull(blocks[i]);
    }

    for (int i = 0; i < NUM_BLOCKS; i++) {
        _free(blocks[i]);
    }
}
END_TEST

START_TEST(allocate_zero_bytes) {
    char *byte = _malloc(0);

    ck_assert_ptr_null(byte);
}
END_TEST

START_TEST(write_to_allocated_memory) {
    int size = 100;
    char *block = _malloc(size);

    ck_assert_ptr_nonnull(block);

    for (int i = 0; i < size; i++) {
        block[i] = 'a'; // Write to allocated memory
    }

    _free(block);
}
END_TEST

START_TEST(free_null_pointer) {
    _free(NULL); // Freeing a NULL pointer should not cause a crash

    ck_assert(1); // If we reach here, the test passed
}
END_TEST

START_TEST(double_free) {
    char *byte = _malloc(1);

    ck_assert_ptr_nonnull(byte);

    _free(byte);
    _free(byte); // Double free, should be handled gracefully

    ck_assert(1); // If we reach here, the test passed
}
END_TEST

START_TEST(fragmented_allocation) {
    char *first = _malloc(10);
    char *second = _malloc(10);

    ck_assert_ptr_nonnull(first);
    ck_assert_ptr_nonnull(second);

    _free(first);
    char *third = _malloc(5); // Allocate after freeing a block

    ck_assert_ptr_nonnull(third);

    _free(second);
    _free(third);
}
END_TEST

START_TEST(reallocate_block) {
    char *block = _malloc(50);
    ck_assert_ptr_nonnull(block);

    block = _realloc(block, 100); // Increase size
    ck_assert_ptr_nonnull(block);

    for (int i = 0; i < 100; i++) {
        block[i] = 'b'; // Write to reallocated memory
    }

    _free(block);
}
END_TEST

START_TEST(allocate_until_failure) {
    char *block;
    int size = 1;

    while (1) {
        block = _malloc(size);
        if (!block)
            break; // Allocation failed

        ck_assert_ptr_nonnull(block);
        _free(block);

        size *= 2; // Exponentially increase the allocation size
    }

    ck_assert(1); // If we reach here, the test passed
}
END_TEST

START_TEST(allocate_free_different_order) {
    char *first = _malloc(10);
    char *second = _malloc(20);

    ck_assert_ptr_nonnull(first);
    ck_assert_ptr_nonnull(second);

    _free(second);
    _free(first);
}
END_TEST

START_TEST(check_memory_overlap) {
    char *first = _malloc(10);
    char *second = _malloc(10);

    ck_assert_ptr_nonnull(first);
    ck_assert_ptr_nonnull(second);

    // Check for overlap
    ck_assert(first + 10 <= second || second + 10 <= first);

    _free(first);
    _free(second);
}
END_TEST

START_TEST(repeated_allocation_freeing) {
    for (int i = 0; i < 1000; i++) {
        char *block = _malloc(100);

        ck_assert_ptr_nonnull(block);
        _free(block);
    }
}
END_TEST

START_TEST(allocate_non_standard_sizes) {
    int sizes[] = {7, 13, 31, 64, 128};
    for (int i = 0; i < 5; i++) {
        char *block = _malloc(sizes[i]);

        ck_assert_ptr_nonnull(block);
        _free(block);
    }
}
END_TEST

START_TEST(free_and_reallocate_loop) {
    char *block = _malloc(10);

    for (int i = 0; i < 100; i++) {
        ck_assert_ptr_nonnull(block);
        _free(block);

        block = _malloc(10);
    }

    _free(block);
}
END_TEST

START_TEST(multithreaded_allocate_free) {
    const int NUM_THREADS = 10;
    pthread_t threads[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }

    ck_assert(1); // If we reach here, the test passed
}
END_TEST

START_TEST(multithreaded_stress_test) {
    const int NUM_THREADS = 10;
    pthread_t threads[NUM_THREADS];

    srand(time(NULL)); // Seed the random number generator

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_create(&threads[i], NULL, thread_function_stress, NULL);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(threads[i], NULL);
    }
    ck_assert(1);
}
END_TEST

START_TEST(check_alignment) {
    void *ptr = _malloc(1);
    ck_assert_ptr_nonnull(ptr);
    ck_assert_uint_eq((uintptr_t)ptr % sizeof(void *), 0); // Check alignment

    _free(ptr);
}
END_TEST

START_TEST(calloc_zero_initialization) {
    int num_elements = 10;
    int *array = (int *)_calloc(num_elements, sizeof(int));
    ck_assert_ptr_nonnull(array);

    for (int i = 0; i < num_elements; i++) {
        ck_assert_int_eq(array[i], 0); // Check if each element is zero-initialized
    }

    _free(array);
}
END_TEST

START_TEST(realloc_smaller_size) {
    char *block = _malloc(100);
    ck_assert_ptr_nonnull(block);

    block = _realloc(block, 50); // Reduce size
    ck_assert_ptr_nonnull(block);

    _free(block);
}
END_TEST

START_TEST(realloc_larger_size_content_preservation) {
    int old_size = 10;
    int new_size = 20;
    int *block = (int *)_malloc(old_size * sizeof(int));

    for (int i = 0; i < old_size; i++) {
        block[i] = i;
    }

    block = (int *)_realloc(block, new_size * sizeof(int));
    ck_assert_ptr_nonnull(block);

    // Check if original content is preserved
    for (int i = 0; i < old_size; i++) {
        ck_assert_int_eq(block[i], i);
    }

    _free(block);
}
END_TEST

START_TEST(realloc_null_pointer) {
    char *block = _realloc(NULL, 10); // Equivalent to malloc(10)
    ck_assert_ptr_nonnull(block);

    _free(block);
}
END_TEST

START_TEST(realloc_zero_size) {
    char *block = _malloc(10);
    ck_assert_ptr_nonnull(block);

    block = _realloc(block, 0); // Equivalent to free(block)
    ck_assert_ptr_null(block);
}
END_TEST

START_TEST(free_in_reverse_order) {
    const int NUM_BLOCKS = 100;
    void *blocks[NUM_BLOCKS];

    for (int i = 0; i < NUM_BLOCKS; i++) {
        blocks[i] = _malloc((i + 1) * 10);
        ck_assert_ptr_nonnull(blocks[i]);
    }

    for (int i = NUM_BLOCKS - 1; i >= 0; i--) {
        _free(blocks[i]);
    }
}
END_TEST

START_TEST(boundary_check) {
    int size = 10;
    char *block = _malloc(size);
    ck_assert_ptr_nonnull(block);

    // Write to the beginning and end of the allocated block
    block[0] = 'a';
    block[size - 1] = 'z';

    ck_assert(block[0] == 'a' && block[size - 1] == 'z');

    _free(block);
}
END_TEST

START_TEST(stress_test_random_operations) {
    srand(time(NULL));
    const int NUM_OPERATIONS = 1000;
    void *blocks[NUM_OPERATIONS] = {NULL};

    for (int i = 0; i < NUM_OPERATIONS; i++) {
        int operation = rand() % 3; // 0: malloc, 1: realloc, 2: free
        int size = rand() % 1000 + 1;

        switch (operation) {
        case 0: // malloc
            blocks[i] = _malloc(size);
            ck_assert_ptr_nonnull(blocks[i]);
            break;
        case 1: // realloc
            if (blocks[i] != NULL) {
                blocks[i] = _realloc(blocks[i], size);
                ck_assert_ptr_nonnull(blocks[i]);
            }
            break;
        case 2: // free
            if (blocks[i] != NULL) {
                _free(blocks[i]);
                blocks[i] = NULL;
            }
            break;
        }
    }

    // Cleanup
    for (int i = 0; i < NUM_OPERATIONS; i++) {
        if (blocks[i] != NULL) {
            _free(blocks[i]);
        }
    }
}
END_TEST

Suite *create_suite() {
    Suite *suite = suite_create("Malloc tests");
    TCase *tc_core = tcase_create("Core");

    tcase_add_test(tc_core, allocate_one_byte_then_free);
    tcase_add_test(tc_core, allocate_large_block);
    tcase_add_test(tc_core, allocate_multiple_small_blocks);
    tcase_add_test(tc_core, allocate_zero_bytes);
    tcase_add_test(tc_core, write_to_allocated_memory);
    tcase_add_test(tc_core, free_null_pointer);
    tcase_add_test(tc_core, double_free);
    tcase_add_test(tc_core, fragmented_allocation);
    tcase_add_test(tc_core, reallocate_block);
    tcase_add_test(tc_core, allocate_until_failure);
    tcase_add_test(tc_core, allocate_free_different_order);
    tcase_add_test(tc_core, check_memory_overlap);
    tcase_add_test(tc_core, repeated_allocation_freeing);
    tcase_add_test(tc_core, allocate_non_standard_sizes);
    tcase_add_test(tc_core, free_and_reallocate_loop);
    tcase_add_test(tc_core, multithreaded_allocate_free);
    tcase_add_test(tc_core, multithreaded_stress_test);
    tcase_add_test(tc_core, check_alignment);
    tcase_add_test(tc_core, calloc_zero_initialization);
    tcase_add_test(tc_core, realloc_smaller_size);
    tcase_add_test(tc_core, realloc_larger_size_content_preservation);
    tcase_add_test(tc_core, realloc_null_pointer);
    tcase_add_test(tc_core, realloc_zero_size);
    tcase_add_test(tc_core, free_in_reverse_order);
    tcase_add_test(tc_core, boundary_check);
    tcase_add_test(tc_core, stress_test_random_operations);

    suite_add_tcase(suite, tc_core);

    return suite;
}

int main(void) {
    Suite *suite = create_suite();
    SRunner *sr;
    int number_failed;

    sr = srunner_create(suite);

    srunner_run_all(sr, CK_NORMAL);

    number_failed = srunner_ntests_failed(sr);

    srunner_free(sr);

    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
