#include "gauss_jordan.h"
#include "augmented.h"
#include "pivot.h"
#include "row_ops.h"

Num_Status gauss_jordan_solve(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double b[NUM_MAX_N],
    double       x[NUM_MAX_N],
    int          n
) {
    /*
     * aug is stack-allocated and owned by this function for its lifetime.
     * A and b are never written; x is written only after NUM_OK is confirmed.
     */
    double aug[NUM_MAX_N][NUM_MAX_N + 1];
    /*
     * C99 does not allow implicit double(*)[N] → const double(*)[N] for
     * array-pointer arguments (-Wpedantic). Use an explicit const alias for
     * all read-only calls so the mutable aug stays the sole write target.
     */
    const double (*caug)[NUM_MAX_N + 1] = (const double (*)[NUM_MAX_N + 1])aug;
    int    col, r, pivot_row, max_row;
    int    has_free_var;
    double factor;

    if (n < 1 || n > NUM_MAX_N) return NUM_ERR_BAD_INPUT;

    num_aug_build(aug, A, b, n);

    pivot_row    = 0;
    has_free_var = 0;

    for (col = 0; col < n && pivot_row < n; ++col) {

        max_row = num_find_max_row(caug, pivot_row, col, n);

        if (max_row == -1) {
            /*
             * No usable pivot in this column — free variable.
             * Continue rather than returning so the full matrix is reduced
             * and the inconsistency check below runs on every row.
             */
            has_free_var = 1;
            continue;
        }

        if (max_row != pivot_row) {
            num_row_swap(aug[pivot_row], aug[max_row], n + 1);
        }

        num_row_scale(aug[pivot_row], 1.0 / aug[pivot_row][col], n + 1);

        for (r = 0; r < n; ++r) {
            if (r == pivot_row) continue;
            factor = aug[r][col];
            num_row_axpy(aug[r], aug[pivot_row], -factor, n + 1);
        }

        ++pivot_row;
    }

    if (num_aug_is_inconsistent(caug, n)) return NUM_ERR_NO_SOLUTION;
    if (has_free_var)                     return NUM_ERR_INFINITE;

    num_aug_extract(x, caug, n);
    return NUM_OK;
}
