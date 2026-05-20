/*
 * row_ops.h — row-level operations on augmented/coefficient matrices.
 *
 * A "row" is a double* with `cols` contiguous elements — the same
 * memory representation as a vec, but named differently so algorithm
 * code reads in terms of rows rather than vectors.
 *
 * Every function delegates to vec.h; this layer exists for readability
 * at the call site, not for new logic.
 *
 * Depends on: vec.h, numerics.h.
 * Memory: all pointers are borrowed; caller retains ownership.
 */

#ifndef ROW_OPS_H
#define ROW_OPS_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Exchange r1[0..cols-1] with r2[0..cols-1]. */
void num_row_swap(double* r1, double* r2, int cols);

/* r[j] *= s  for j in [0, cols). */
void num_row_scale(double* row, double s, int cols);

/* dst[j] += s * src[j]  for j in [0, cols). */
void num_row_axpy(double* dst, const double* src, double s, int cols);

/*
 * Backward-compatible aliases (used by the Python bridge and existing tests).
 * These resolve to the same symbols at link time via inline forwarding in the
 * .c file — no separate object code is generated.
 */
void num_swap_rows(double* r1, double* r2, int cols);
void num_scale_row(double* row, double s, int cols);
void num_axpy_row(double* dst, const double* src, double s, int cols);

#ifdef __cplusplus
}
#endif

#endif /* ROW_OPS_H */
