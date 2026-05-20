#include "mat.h"
#include "vec.h"

void num_mat_vec(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double* x,
    double*       y,
    int           n
) {
    int i;
    for (i = 0; i < n; ++i) {
        y[i] = num_vec_dot(A[i], x, n);
    }
}

void num_mat_zero(double A[NUM_MAX_N][NUM_MAX_N], int n)
{
    int i;
    for (i = 0; i < n; ++i) num_vec_zero(A[i], n);
}

void num_mat_identity(double A[NUM_MAX_N][NUM_MAX_N], int n)
{
    int i, j;
    for (i = 0; i < n; ++i) {
        for (j = 0; j < n; ++j) A[i][j] = (i == j) ? 1.0 : 0.0;
    }
}

void num_mat_copy(
    double       dst[NUM_MAX_N][NUM_MAX_N],
    const double src[NUM_MAX_N][NUM_MAX_N],
    int          n
) {
    int i;
    for (i = 0; i < n; ++i) num_vec_copy(dst[i], src[i], n);
}
