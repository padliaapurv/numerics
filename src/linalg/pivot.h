/*
 * pivot.h — pivot selection for elimination algorithms.
 *
 * Pure read: no row is modified.  Callers use the returned index to decide
 * whether and how to swap rows before proceeding with elimination.
 *
 * Depends on: scalar.h, numerics.h.
 * Memory: the augmented matrix pointer is borrowed; caller owns it.
 */

#ifndef PIVOT_H
#define PIVOT_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * num_find_max_row
 *
 * Scan aug[start_row..n-1][col] and return the index of the row
 * whose absolute value is largest.
 *
 * Returns -1 if every candidate value is near-zero (no usable pivot).
 *
 * Parameters:
 *   aug        Augmented matrix [NUM_MAX_N][NUM_MAX_N+1]. Read-only.
 *   start_row  First row to consider (rows above are already pivoted).
 *   col        Column being inspected.
 *   n          Active system dimension.
 */
int num_find_max_row(
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          start_row,
    int          col,
    int          n
);

#ifdef __cplusplus
}
#endif

#endif /* PIVOT_H */
