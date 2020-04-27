#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "common_types.h"
#include "ADTList.h"
#define N 10000

int main(int argc, char **argv) {
    // mode == 0 αν real, 1 αν amortized
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
    // total ο συνολικός αριθμός βημάτων
    int total = 0;
    List list = list_create(NULL);
    list_insert_next(list, list_first(list), NULL);
    total += list_steps(list);
    // τα amortized/real έχουν μια "απόσταση" ενός βήματος, αφού το n έχει διαφορετική σημασία για το καθένα
    // (η amortized μετράει τον μέσο όρο μέχρι και n-οστό στοιχείο, ενώ η real μετράει τα βήματα αν υπάρχουν ήδη n στοιχεια)
    // άρα η amortized θα εκτελέσει μόνη της το πρώτο βήμα και η real μόνη της το τελευταίο, από συνολικά Ν + 1 βήματα
    if (mode) {
        printf("%d,%d\n", 1, total);
    }
    for (uint i = 1 ; i <= N - 1 ; i++) {
        list_insert_next(list, list_first(list), NULL);
        total += list_steps(list);
        printf("%d,%d\n", mode ? i + 1 : i, mode ? total / (i + 1) : list_steps(list));
    }
    list_insert_next(list, list_first(list), NULL);
    if (!mode) {
        printf("%d,%d\n", N, list_steps(list));
    }
    list_destroy(list);
    return 0;
}