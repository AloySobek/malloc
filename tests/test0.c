#include <stdlib.h>

int main(void) {
    int i;
    char *addr = malloc(64);
    free(addr);
    addr = malloc(1024);
    free(addr);

    i = 0;
    while (i < 1024) {
        i++;
    }
    return (0);
}
