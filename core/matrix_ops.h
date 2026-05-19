/*
 * matrix_ops.h
 *
 * Shared, safety-critical row and vector primitives.
 *
 * All functions operate on plain C arrays passed by pointer.
 * Using double* for rows (instead of 2-D array types) keeps the interface
 * independent of the outer array's compile-time column count, so every
 * algorithm can reuse these routines regardless of its internal layout.
 *
 * Caller is responsible for ensuring all lengths satisfy n <= NUM_MAX_N.
 */

#ifndef MATRIX_OPS_H
#define MATRIX_OPS_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Row operations (in-place, operate on a single row as double*) ────────── */

/* Swap every element of r1[0..cols-1] with r2[0..cols-1]. */
void num_swap_rows(double* r1, double* r2, int cols);

/* Scale: row[j] *= scalar  for j in [0, cols). */
void num_scale_row(double* row, double scalar, int cols);

/* AXPY: dst[j] += scalar * src[j]  for j in [0, cols). */
void num_axpy_row(double* dst, const double* src, double scalar, int cols);

/* ── Vector operations ────────────────────────────────────────────────────── */

/* Dot product: sum of a[i] * b[i] for i in [0, n). */
double num_dot(const double* a, const double* b, int n);

/* Infinity norm: max |v[i]| for i in [0, n). */
double num_norm_inf(const double* v, int n);

/* ── Matrix-vector product ────────────────────────────────────────────────── */

/*
 * y = A * x  for an n x n matrix A stored as A[NUM_MAX_N][NUM_MAX_N].
 * Result written to y[0..n-1].  n must satisfy 1 <= n <= NUM_MAX_N.
 */
void num_mat_vec(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double* x,
    double*       y,
    int           n
);

#ifdef __cplusplus
}
#endif

#endif /* MATRIX_OPS_H */
