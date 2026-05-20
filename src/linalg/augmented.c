#include "augmented.h"
#include "vec.h"
#include "scalar.h"

void num_aug_build(
    double       aug[NUM_MAX_N][NUM_MAX_N + 1],
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double b[NUM_MAX_N],
    int          n
) {
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) aug[i][j] = A[i][j];
        aug[i][n] = b[i];
        /* Zero any padding beyond column n. */
        for (j = n + 1; j <= NUM_MAX_N; ++j) aug[i][j] = 0.0;
    }
}

int num_aug_is_inconsistent(
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          n
) {
    int i;
    for (i = 0; i < n; ++i) {
        if (num_vec_is_zero(aug[i], n) && !num_near_zero(aug[i][n])) {
            return 1;
        }
    }
    return 0;
}

void num_aug_extract(
    double       x[NUM_MAX_N],
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          n
) {
    int i;
    for (i = 0; i < n; ++i) x[i] = aug[i][n];
}
