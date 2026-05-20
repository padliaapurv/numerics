/*
 * vec.h — vector (1-D array) operations.
 *
 * All functions operate on a double* with an explicit element count n.
 * Callers own every pointer passed in; nothing is allocated here.
 * Depends on: scalar.h, numerics.h.
 */

#ifndef VEC_H
#define VEC_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/* ── Reductions ──────────────────────────────────────────────────────────── */

/* sum(a[i] * b[i]) */
double num_vec_dot(const double* a, const double* b, int n);

/* max |v[i]| */
double num_vec_norm_inf(const double* v, int n);

/* sum |v[i]| */
double num_vec_norm_1(const double* v, int n);

/* sum v[i]^2  (squared Euclidean norm; avoids sqrt) */
double num_vec_norm_2sq(const double* v, int n);

/* 1 if all |v[i]| < NUM_EPSILON, else 0. */
int num_vec_is_zero(const double* v, int n);

/* ── Mutations (in-place) ────────────────────────────────────────────────── */

/* v[i] *= s */
void num_vec_scale(double* v, double s, int n);

/* dst[i] += s * src[i] */
void num_vec_axpy(double* dst, const double* src, double s, int n);

/* dst[i] = src[i] */
void num_vec_copy(double* dst, const double* src, int n);

/* v[i] = 0 */
void num_vec_zero(double* v, int n);

/* Exchange a[i] <-> b[i] */
void num_vec_swap(double* a, double* b, int n);

#ifdef __cplusplus
}
#endif

#endif /* VEC_H */
