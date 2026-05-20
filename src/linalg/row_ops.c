#include "row_ops.h"
#include "vec.h"

void num_row_swap(double* r1, double* r2, int cols)
{
    num_vec_swap(r1, r2, cols);
}

void num_row_scale(double* row, double s, int cols)
{
    num_vec_scale(row, s, cols);
}

void num_row_axpy(double* dst, const double* src, double s, int cols)
{
    num_vec_axpy(dst, src, s, cols);
}

/* ── Backward-compatible aliases ─────────────────────────────────────────── */

void num_swap_rows(double* r1, double* r2, int cols) { num_row_swap(r1, r2, cols); }
void num_scale_row(double* row, double s, int cols)  { num_row_scale(row, s, cols); }
void num_axpy_row(double* dst, const double* src, double s, int cols)
{
    num_row_axpy(dst, src, s, cols);
}
