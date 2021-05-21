/**
 * File              : malloc.h
 * Author            : Rustam Khafizov <super.rustamm@gmail.com>
 * Date              : 21.05.2021 16:28
 * Last Modified Date: 21.05.2021 16:30
 * Last Modified By  : Rustam Khafizov <super.rustamm@gmail.com>
 */

#ifndef MALLOC_H
# define MALLOC_H

# include <stdio.h>

void free(void *ptr);
void *malloc(size_t size);
void *realloc(void *ptr, size_t size);

#endif
