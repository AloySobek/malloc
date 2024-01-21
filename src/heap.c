#include "malloc.h"

struct pool _pool = {{100, 64, 100, 1024, 32, 1, 1024, 0}, {0}, {0}, NULL, NULL, 0};
pthread_mutex_t _mutex = PTHREAD_MUTEX_INITIALIZER;

static struct doubly_linked_list_node *_insert_node(struct doubly_linked_list_node *head,
                                                    struct doubly_linked_list_node *node) {
    if (node == NULL) {
        return head;
    }

    if (head == NULL) {
        node->next = node;
        node->prev = node;

        return node;
    } else {
        node->next = head;
        node->prev = head->prev;

        node->prev->next = node;
        node->next->prev = node;

        return head;
    }
}

static struct doubly_linked_list_node *_remove_node(struct doubly_linked_list_node *head,
                                                    struct doubly_linked_list_node **node) {
    if (head == NULL) {
        return NULL;
    }

    if (*node == NULL) {
        *node = head->prev;
    } else if (head == *node) {
        head = (*node)->next;
    }

    (*node)->prev->next = (*node)->next;
    (*node)->next->prev = (*node)->prev;

    (*node)->next = NULL;
    (*node)->prev = NULL;

    if (head == *node) {
        return NULL;
    }

    return head;
}

static struct cluster *_allocate_cluster(enum type type, size_t n, size_t size) {
    size_t aligned_size =
        (sizeof(struct cluster) + (sizeof(struct block) * n) + (n * size) + (getpagesize() - 1)) &
        ~(getpagesize() - 1);

    struct cluster *cluster =
        mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

    if (cluster == MAP_FAILED) {
        return NULL;
    }

    cluster->node.next = NULL;
    cluster->node.prev = NULL;
    cluster->available_blocks = NULL;
    cluster->occupied_blocks = NULL;
    cluster->capacity = n;
    cluster->size = n;

    void *block = cluster + 1;

    for (size_t i = 0; i < n; ++i, block += sizeof(struct block) + size) {
        ((struct block *)block)->node.next = NULL;
        ((struct block *)block)->node.prev = NULL;
        ((struct block *)block)->cluster = cluster;
        ((struct block *)block)->size = 0;
        ((struct block *)block)->type = type;

        cluster->available_blocks = _INSERT_BLOCK(cluster->available_blocks, block);
    }

    return cluster;
}

struct block *_get_block(size_t size) {
    struct heap *heap = NULL;
    struct cluster *cluster = NULL;
    struct block *block = NULL;
    size_t n = 0, s = 0;
    enum type type = MaxBlock;

    if (size <= _pool.cfg.tiny_size) {
        heap = &_pool.tiny;
        n = _pool.cfg.tiny_n;
        s = _pool.cfg.tiny_size;
        type = TinyBlock;
    } else if (size <= _pool.cfg.small_size) {
        heap = &_pool.small;
        n = _pool.cfg.small_n;
        s = _pool.cfg.small_size;
        type = SmallBlock;
    }

    if (heap) {
        heap->available_clusters = _REMOVE_BLOCK(heap->available_clusters, cluster);

        if (!cluster && heap->capacity / n < _pool.cfg.max_clusters) {
            if ((cluster = _allocate_cluster(type, n, s))) {
                heap->capacity += n;
                heap->size += n;
            }
        }

        if (cluster) {
            cluster->available_blocks = _REMOVE_BLOCK(cluster->available_blocks, block);
            cluster->occupied_blocks = _INSERT_BLOCK(cluster->occupied_blocks, block);

            if (cluster->available_blocks) {
                heap->available_clusters = _INSERT_BLOCK(heap->available_clusters, cluster);
            } else {
                heap->occupied_clusters = _INSERT_BLOCK(heap->occupied_clusters, cluster);
            }

            cluster->size -= 1;
            heap->size -= 1;
        }
    }

    if (!block) {
        size_t aligned_size =
            (sizeof(struct block) + size + (getpagesize() - 1)) & ~(getpagesize() - 1);

        block = mmap(NULL, aligned_size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0);

        if (block == MAP_FAILED) {
            return NULL;
        }

        block->node.next = NULL;
        block->node.prev = NULL;
        block->cluster = NULL;
        block->type = LargeBlock;

        _pool.large = _INSERT_BLOCK(_pool.large, block);
    }

    block->size = size;

    return block;
}

void _return_block(struct block *block) {
    struct heap *heap = NULL;
    size_t n = 0, s = 0;

    switch (block->type) {
    case TinyBlock: {
        heap = &_pool.tiny;
        n = _pool.cfg.tiny_n;
        s = _pool.cfg.tiny_size;

        break;
    }
    case SmallBlock: {
        heap = &_pool.small;
        n = _pool.cfg.small_n;
        s = _pool.cfg.small_size;

        break;
    }
    case LargeBlock: {
        _pool.large = _REMOVE_BLOCK(_pool.large, block);

        munmap(block, sizeof(struct block) + block->size);

        return;
    }
    default: {
        break;
    }
    }

    block->cluster->occupied_blocks = _REMOVE_BLOCK(block->cluster->occupied_blocks, block);
    block->cluster->available_blocks = _INSERT_BLOCK(block->cluster->available_blocks, block);

    if (!block->cluster->size) {
        heap->occupied_clusters = (struct cluster *)_remove_node(
            (struct doubly_linked_list_node *)heap->occupied_clusters,
            (struct doubly_linked_list_node **)&block->cluster);

        heap->available_clusters = (struct cluster *)_insert_node(
            (struct doubly_linked_list_node *)heap->available_clusters,
            (struct doubly_linked_list_node *)block->cluster);
    }

    block->cluster->size += 1;
    heap->size += 1;

    if (!block->cluster->occupied_blocks && heap->size / n > _pool.cfg.min_available_clusters) {
        heap->available_clusters = (struct cluster *)_remove_node(
            (struct doubly_linked_list_node *)heap->available_clusters,
            (struct doubly_linked_list_node **)&block->cluster);

        munmap(block->cluster, sizeof(struct cluster) + (sizeof(struct block) * n) + (n * s));
    }
}
