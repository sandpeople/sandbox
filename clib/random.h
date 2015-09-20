
#include <stdlib.h>
#include <time.h>

__attribute__((constructor)) static void random_init() {
    srand(time(NULL));
}

double rand0to1() {
    return (double)rand() / (double)RAND_MAX;
}

