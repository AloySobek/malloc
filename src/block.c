#include "malloc.h"

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
