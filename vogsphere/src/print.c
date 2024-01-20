#include "malloc.h"

#include <stdio.h>

static void ft_putchar(const char c) {
    if (c >= 0) {
        write(1, &c, 1);
    }
}

static void ft_putstr(const char *s) {
    if (s) {
        while (*s) {
            ft_putchar(*s++);
        }
    }
}

static void ft_itoa_base(size_t nb, char base, char length, char prefix) {
    char *str;

    str = "0123456789ABCDEFGHIJKLMNOPQRSTUIVWXYZ";

    if (nb / base) {
        ft_itoa_base(nb / base, base, length - 1, prefix);
    } else {
        if (prefix) {
            ft_putstr("0x");
        }
        while (--length > 0) {
            ft_putstr("0");
        }
    }

    write(1, &str[nb % base], 1);
}

size_t print_block(struct block *head, char available) {
    size_t total = 0;

    struct block *block = head;

    if (block) {
        do {
            available ? ft_putstr("(+) ") : ft_putstr("(-) ");
            ft_itoa_base((size_t)(block + 1), 16, 9, 1);
            ft_putstr(" - ");
            ft_itoa_base((size_t)((void *)(block + 1) + block->size), 16, 9, 1);
            ft_putstr(" : ");
            ft_itoa_base(block->size, 10, 0, 0);
            ft_putstr(" bytes\n");

            total += block->size;

            block = (struct block *)block->node.next;
        } while (block != head);
    }

    return total;
}

size_t print_cluster(struct cluster *head, const char *type) {
    size_t total = 0;

    struct cluster *cluster = head;

    if (cluster) {
        do {

            ft_putstr(type);
            ft_putstr(" : ");
            ft_itoa_base((size_t)(cluster + 1), 16, 9, 1);
            ft_putchar('\n');

            total += print_block(cluster->available_blocks, 1);
            total += print_block(cluster->occupied_blocks, 0);

            cluster = (struct cluster *)cluster->node.next;
        } while (cluster != head);
    }

    return total;
}

void show_alloc_mem() {
    size_t total = 0;

    total += print_cluster(_pool.tiny.available_clusters, "(+) TINY");
    total += print_cluster(_pool.tiny.occupied_clusters, "(-) TINY");
    total += print_cluster(_pool.small.available_clusters, "(+) SMALL");
    total += print_cluster(_pool.small.occupied_clusters, "(-) SMALL");

    if (_pool.large) {
        ft_putstr("(-) LARGE");
        ft_putstr(" : ");
        ft_itoa_base((size_t)_pool.large, 16, 9, 1);
        ft_putchar('\n');

        total += print_block(_pool.large, 0);
    }

    ft_putstr("Total : ");
    ft_itoa_base(total, 10, 0, 0);
    ft_putchar('\n');
}

void show_alloc_mem_ex() {
    show_alloc_mem();

    ft_putstr("History of allocations:\n");

    for (size_t i = 1; _pool.n - (long long)i >= 0 && i < _pool.cfg.max_records; ++i) {
        ft_putstr("\tType: ");

        switch (_pool.records[(_pool.n - i) % _pool.cfg.max_records].block_type) {
        case TinyBlock: {
            ft_putstr("Tiny;  Size: ");

            break;
        }
        case SmallBlock: {
            ft_putstr("Small; Size: ");

            break;
        }
        case LargeBlock: {
            ft_putstr("Large: Size: ");

            break;
        }
        default: {
            break;
        }
        }

        ft_itoa_base(_pool.records[(_pool.n - i) % _pool.cfg.max_records].size, 10, 0, 0);

        ft_putstr(" bytes\n");
    }
}
