/*
 * mat.h — whole-matrix operations.
 *
 * mat_vec is the workhorse used by residual checks and iterative methods.
 * mat_zero / mat_identity / mat_copy are setup utilities.
 *
 * Depends on: vec.h, numerics.h.
 * Memory: all pointers borrowed; caller owns the matrices.
 */

#ifndef MAT_H
#define MAT_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * y = A * x   (n × n matrix times n-vector).
 * y is write-only; A and x are read-only.
 * n must satisfy 1 <= n <= NUM_MAX_N.
 *
 * Backward-compatible alias: num_mat_vec (same symbol, same signature).
 */
void num_mat_vec(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double* x,
    double*       y,
    int           n
);

/* A[i][j] = 0  for all i, j in [0, n). */
void num_mat_zero(double A[NUM_MAX_N][NUM_MAX_N], int n);

/* A = I_n  (identity; only the n×n active block is touched). */
void num_mat_identity(double A[NUM_MAX_N][NUM_MAX_N], int n);

/* dst[i][j] = src[i][j]  for i, j in [0, n). */
void num_mat_copy(
    double       dst[NUM_MAX_N][NUM_MAX_N],
    const double src[NUM_MAX_N][NUM_MAX_N],
    int          n
);

#ifdef __cplusplus
}
#endif

#endif /* MAT_H */
