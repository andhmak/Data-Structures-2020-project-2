#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_types.h"
#include "ADTVector.h"
#define N 10000

int main(int argc, char **argv) {
    char mode;
    if (argc != 2) {
        fprintf(stderr, "Invalid number of arguments\n");
        return 1;
    }
    if (!strcmp(argv[1], "real")) {
        mode = 0;
    }
    else if (!strcmp(argv[1], "amortized")) {
        mode = 1;
    }
    else {
        fprintf(stderr, "Expected \"real\" or \"amortized\" as argument\n");
        return 2;
    }
    int total = 0;
    Vector vec = vector_create(0, NULL);
    vector_insert_last(vec, NULL);
    total += vector_steps(vec);
    if (mode) {
        printf("%d,%d\n", 1, total);
    }
    for (uint i = 1 ; i <= N - 1 ; i++) {
        vector_insert_last(vec, NULL);
        total += vector_steps(vec);
        printf("%d,%d\n", mode ? i + 1 : i, mode ? total / (i + 1) : vector_steps(vec));
    }
    vector_insert_last(vec, NULL);
    if (!mode) {
        printf("%d,%d\n", N, vector_steps(vec));
    }
    vector_destroy(vec);
    return 0;
}