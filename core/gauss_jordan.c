#include "gauss_jordan.h"
#include "matrix_ops.h"
#include <math.h>   /* fabs */

Num_Status gauss_jordan_solve(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double b[NUM_MAX_N],
    double       x[NUM_MAX_N],
    int          n
) {
    /*
     * aug[i][0..n-1] = row i of A
     * aug[i][n]      = b[i]
     */
    double aug[NUM_MAX_N][NUM_MAX_N + 1];
    int    i, j, r, pivot_row, max_row, col;
    int    has_free_var;
    double max_val, factor;

    if (n < 1 || n > NUM_MAX_N) {
        return NUM_ERR_BAD_INPUT;
    }

    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) { aug[i][j] = A[i][j]; }
        aug[i][n] = b[i];
    }

    pivot_row    = 0;
    has_free_var = 0;

    for (col = 0; col < n && pivot_row < n; ++col) {

        /* Partial pivoting: bring the row with the largest magnitude to front. */
        max_row = pivot_row;
        max_val = fabs(aug[pivot_row][col]);
        for (r = pivot_row + 1; r < n; ++r) {
            double val = fabs(aug[r][col]);
            if (val > max_val) { max_val = val; max_row = r; }
        }

        if (max_val < NUM_EPSILON) {
            /*
             * No usable pivot — free variable present.
             * Continue (do not return) so the full matrix is reduced
             * and the inconsistency check below runs on all rows.
             */
            has_free_var = 1;
            continue;
        }

        if (max_row != pivot_row) {
            num_swap_rows(aug[pivot_row], aug[max_row], n + 1);
        }

        /* Normalise the pivot row so aug[pivot_row][col] == 1. */
        num_scale_row(aug[pivot_row], 1.0 / aug[pivot_row][col], n + 1);

        /* Zero out every other row in this column. */
        for (r = 0; r < n; ++r) {
            if (r == pivot_row) { continue; }
            factor = aug[r][col];
            num_axpy_row(aug[r], aug[pivot_row], -factor, n + 1);
        }

        ++pivot_row;
    }

    /*
     * Inconsistency check: a row [0 ... 0 | c != 0] means no solution.
     * Must run after full elimination — premature return would miss rows
     * that still carry a non-zero RHS hidden behind unreduced leading terms.
     */
    for (i = 0; i < n; ++i) {
        int all_zero = 1;
        for (j = 0; j < n; ++j) {
            if (fabs(aug[i][j]) > NUM_EPSILON) { all_zero = 0; break; }
        }
        if (all_zero && fabs(aug[i][n]) > NUM_EPSILON) {
            return NUM_ERR_NO_SOLUTION;
        }
    }

    if (has_free_var) {
        return NUM_ERR_INFINITE;
    }

    for (i = 0; i < n; ++i) { x[i] = aug[i][n]; }

    return NUM_OK;
}
