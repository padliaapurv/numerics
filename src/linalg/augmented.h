/*
 * augmented.h — augmented matrix [A | b] lifecycle.
 *
 * Owns the three distinct phases of an augmented matrix:
 *   1. Construction   — build [A | b] from separate A and b
 *   2. Interrogation  — check for inconsistency after elimination
 *   3. Extraction     — read the solution column back into x
 *
 * The augmented matrix itself is always owned by the calling solver
 * (stack-allocated). These functions only borrow the pointer.
 *
 * Depends on: vec.h, scalar.h, numerics.h.
 */

#ifndef AUGMENTED_H
#define AUGMENTED_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * num_aug_build
 *
 * Fill aug[i][0..n-1] from A[i][*] and aug[i][n] from b[i].
 * Entries in aug beyond column n are zeroed.
 * A and b are read-only; aug is write-only.
 */
void num_aug_build(
    double       aug[NUM_MAX_N][NUM_MAX_N + 1],
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double b[NUM_MAX_N],
    int          n
);

/*
 * num_aug_is_inconsistent
 *
 * Returns 1 if any row i has:  all aug[i][0..n-1] near-zero  AND  |aug[i][n]| > epsilon.
 * A row like this represents "0 = c" (c != 0), which is a contradiction.
 * Call this after full elimination; partial reduction may miss it.
 */
int num_aug_is_inconsistent(
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          n
);

/*
 * num_aug_extract
 *
 * Copy the RHS column: x[i] = aug[i][n] for i in [0, n).
 * Call only when the system is known to have a unique solution.
 */
void num_aug_extract(
    double       x[NUM_MAX_N],
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          n
);

#ifdef __cplusplus
}
#endif

#endif /* AUGMENTED_H */
