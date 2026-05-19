#include "matrix_ops.h"
#include "numerics.h"
#include <math.h>   /* fabs */

/* ── Num_Status string (lives here as the shared utility translation unit) ── */

const char* num_status_str(Num_Status s)
{
    switch (s) {
        case NUM_OK:              return "OK";
        case NUM_ERR_BAD_INPUT:   return "ERROR: bad input";
        case NUM_ERR_NO_SOLUTION: return "ERROR: no solution (inconsistent system)";
        case NUM_ERR_INFINITE:    return "ERROR: infinite solutions (underdetermined)";
        case NUM_ERR_SINGULAR:    return "ERROR: singular or near-singular matrix";
        default:                  return "ERROR: unknown status";
    }
}

/* ── Row operations ──────────────────────────────────────────────────────── */

void num_swap_rows(double* r1, double* r2, int cols)
{
    int j;
    double tmp;
    for (j = 0; j < cols; ++j) {
        tmp   = r1[j];
        r1[j] = r2[j];
        r2[j] = tmp;
    }
}

void num_scale_row(double* row, double scalar, int cols)
{
    int j;
    for (j = 0; j < cols; ++j) {
        row[j] *= scalar;
    }
}

void num_axpy_row(double* dst, const double* src, double scalar, int cols)
{
    int j;
    for (j = 0; j < cols; ++j) {
        dst[j] += scalar * src[j];
    }
}

/* ── Vector operations ───────────────────────────────────────────────────── */

double num_dot(const double* a, const double* b, int n)
{
    int i;
    double sum = 0.0;
    for (i = 0; i < n; ++i) {
        sum += a[i] * b[i];
    }
    return sum;
}

double num_norm_inf(const double* v, int n)
{
    int i;
    double max = 0.0;
    for (i = 0; i < n; ++i) {
        double val = fabs(v[i]);
        if (val > max) { max = val; }
    }
    return max;
}

/* ── Matrix-vector product ───────────────────────────────────────────────── */

void num_mat_vec(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double* x,
    double*       y,
    int           n
) {
    int i, j;
    for (i = 0; i < n; ++i) {
        y[i] = 0.0;
        for (j = 0; j < n; ++j) {
            y[i] += A[i][j] * x[j];
        }
    }
}
