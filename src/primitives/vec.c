#include "vec.h"
#include "scalar.h"
#include <math.h>

double num_vec_dot(const double* a, const double* b, int n)
{
    int i;
    double s = 0.0;
    for (i = 0; i < n; ++i) s += a[i] * b[i];
    return s;
}

double num_vec_norm_inf(const double* v, int n)
{
    int i;
    double m = 0.0;
    for (i = 0; i < n; ++i) {
        double av = fabs(v[i]);
        if (av > m) m = av;
    }
    return m;
}

double num_vec_norm_1(const double* v, int n)
{
    int i;
    double s = 0.0;
    for (i = 0; i < n; ++i) s += fabs(v[i]);
    return s;
}

double num_vec_norm_2sq(const double* v, int n)
{
    return num_vec_dot(v, v, n);
}

int num_vec_is_zero(const double* v, int n)
{
    int i;
    for (i = 0; i < n; ++i) {
        if (!num_near_zero(v[i])) return 0;
    }
    return 1;
}

void num_vec_scale(double* v, double s, int n)
{
    int i;
    for (i = 0; i < n; ++i) v[i] *= s;
}

void num_vec_axpy(double* dst, const double* src, double s, int n)
{
    int i;
    for (i = 0; i < n; ++i) dst[i] += s * src[i];
}

void num_vec_copy(double* dst, const double* src, int n)
{
    int i;
    for (i = 0; i < n; ++i) dst[i] = src[i];
}

void num_vec_zero(double* v, int n)
{
    int i;
    for (i = 0; i < n; ++i) v[i] = 0.0;
}

void num_vec_swap(double* a, double* b, int n)
{
    int i;
    double t;
    for (i = 0; i < n; ++i) {
        t    = a[i];
        a[i] = b[i];
        b[i] = t;
    }
}
