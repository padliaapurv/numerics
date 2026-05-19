/*
 * test_matrix_ops.cpp  — NOT safety-critical; STL/iostream permitted.
 */

#include "matrix_ops.h"
#include "numerics.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <functional>

/* ── Minimal test framework ─────────────────────────────────────────────── */

static int g_pass = 0, g_fail = 0;

static void run(const std::string& name, std::function<bool(std::string&)> fn)
{
    std::string detail;
    bool ok = fn(detail);
    if (ok) ++g_pass; else ++g_fail;
    std::cout << (ok ? "  [PASS]" : "  [FAIL]") << "  " << name;
    if (!ok) std::cout << "\n         " << detail;
    std::cout << "\n";
}

static bool near(double a, double b, double tol = 1e-12)
{
    return std::fabs(a - b) < tol;
}

/* ── num_swap_rows ───────────────────────────────────────────────────────── */

static void test_swap_basic()
{
    run("swap_rows: contents exchanged", [](std::string& e) {
        double r1[] = {1, 2, 3};
        double r2[] = {4, 5, 6};
        num_swap_rows(r1, r2, 3);
        if (!near(r1[0],4)||!near(r1[1],5)||!near(r1[2],6)) { e="r1 wrong"; return false; }
        if (!near(r2[0],1)||!near(r2[1],2)||!near(r2[2],3)) { e="r2 wrong"; return false; }
        return true;
    });
}

static void test_swap_idempotent()
{
    run("swap_rows: double-swap restores original", [](std::string& e) {
        double r1[] = {7, -2, 0.5};
        double r2[] = {3,  1, 9.0};
        double orig1[3], orig2[3];
        std::memcpy(orig1, r1, sizeof(r1));
        std::memcpy(orig2, r2, sizeof(r2));
        num_swap_rows(r1, r2, 3);
        num_swap_rows(r1, r2, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(r1[i], orig1[i]) || !near(r2[i], orig2[i])) {
                e = "not restored after double swap"; return false;
            }
        return true;
    });
}

/* ── num_scale_row ───────────────────────────────────────────────────────── */

static void test_scale_basic()
{
    run("scale_row: each element multiplied", [](std::string& e) {
        double r[] = {1, 2, 4};
        num_scale_row(r, 0.5, 3);
        if (!near(r[0],0.5)||!near(r[1],1.0)||!near(r[2],2.0)) {
            e = "scaled values wrong"; return false;
        }
        return true;
    });
}

static void test_scale_by_one()
{
    run("scale_row: scale by 1.0 is identity", [](std::string& e) {
        double r[] = {3.14, -2.71, 0.0};
        double orig[3];
        std::memcpy(orig, r, sizeof(r));
        num_scale_row(r, 1.0, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(r[i], orig[i])) { e = "changed after *1"; return false; }
        return true;
    });
}

static void test_scale_inverse()
{
    run("scale_row: scale then inverse-scale restores values", [](std::string& e) {
        double r[] = {6, -3, 9};
        num_scale_row(r, 2.0, 3);
        num_scale_row(r, 0.5, 3);
        if (!near(r[0],6)||!near(r[1],-3)||!near(r[2],9)) {
            e = "not restored"; return false;
        }
        return true;
    });
}

/* ── num_axpy_row ────────────────────────────────────────────────────────── */

static void test_axpy_basic()
{
    run("axpy_row: dst += 2 * src", [](std::string& e) {
        double dst[] = {1, 2, 3};
        const double src[] = {10, 20, 30};
        num_axpy_row(dst, src, 2.0, 3);
        if (!near(dst[0],21)||!near(dst[1],42)||!near(dst[2],63)) {
            e = "axpy result wrong"; return false;
        }
        return true;
    });
}

static void test_axpy_zero_scalar()
{
    run("axpy_row: scalar=0 leaves dst unchanged", [](std::string& e) {
        double dst[] = {5, 6, 7};
        const double src[] = {100, 200, 300};
        double orig[3];
        std::memcpy(orig, dst, sizeof(dst));
        num_axpy_row(dst, src, 0.0, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(dst[i], orig[i])) { e = "dst changed with scalar=0"; return false; }
        return true;
    });
}

static void test_axpy_negation()
{
    run("axpy_row: dst -= src  zeroes dst when dst==src values", [](std::string& e) {
        double dst[] = {3, -1, 7};
        const double src[] = {3, -1, 7};
        num_axpy_row(dst, src, -1.0, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(dst[i], 0.0)) { e = "expected zero row"; return false; }
        return true;
    });
}

/* ── num_dot ─────────────────────────────────────────────────────────────── */

static void test_dot_basic()
{
    run("dot: [1,2,3]·[4,5,6] = 32", [](std::string& e) {
        const double a[] = {1,2,3};
        const double b[] = {4,5,6};
        double d = num_dot(a, b, 3);
        if (!near(d, 32.0)) { e = "dot != 32"; return false; }
        return true;
    });
}

static void test_dot_orthogonal()
{
    run("dot: orthogonal vectors → 0", [](std::string& e) {
        const double a[] = {1, 0};
        const double b[] = {0, 1};
        if (!near(num_dot(a, b, 2), 0.0)) { e = "dot != 0"; return false; }
        return true;
    });
}

static void test_dot_self_is_norm_sq()
{
    run("dot: v·v = ||v||²", [](std::string& e) {
        const double v[] = {3, 4};
        double d = num_dot(v, v, 2);
        if (!near(d, 25.0)) { e = "3²+4² != 25"; return false; }
        return true;
    });
}

/* ── num_norm_inf ────────────────────────────────────────────────────────── */

static void test_norm_inf_basic()
{
    run("norm_inf: max |v[i]|", [](std::string& e) {
        const double v[] = {1, -5, 3, -2};
        if (!near(num_norm_inf(v, 4), 5.0)) { e = "norm != 5"; return false; }
        return true;
    });
}

static void test_norm_inf_zero()
{
    run("norm_inf: zero vector → 0", [](std::string& e) {
        const double v[] = {0, 0, 0};
        if (!near(num_norm_inf(v, 3), 0.0)) { e = "norm != 0"; return false; }
        return true;
    });
}

/* ── num_mat_vec ─────────────────────────────────────────────────────────── */

static void test_mat_vec_identity()
{
    run("mat_vec: I*x = x", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        for (int i = 0; i < 3; ++i) A[i][i] = 1.0;
        const double x[] = {7, -2, 4};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(y[i], x[i])) { e = "I*x != x"; return false; }
        return true;
    });
}

static void test_mat_vec_known()
{
    run("mat_vec: [[1,2],[3,4]]*[1,1] = [3,7]", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        A[0][0]=1; A[0][1]=2;
        A[1][0]=3; A[1][1]=4;
        const double x[] = {1, 1};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 2);
        if (!near(y[0], 3.0) || !near(y[1], 7.0)) { e = "result wrong"; return false; }
        return true;
    });
}

static void test_mat_vec_zero_matrix()
{
    run("mat_vec: 0*x = 0", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        const double x[] = {5, -3, 1};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 3);
        for (int i = 0; i < 3; ++i)
            if (!near(y[i], 0.0)) { e = "0*x != 0"; return false; }
        return true;
    });
}

/* ── num_status_str ──────────────────────────────────────────────────────── */

static void test_status_str_never_null()
{
    run("num_status_str: never returns NULL for any Num_Status", [](std::string& e) {
        Num_Status codes[] = {
            NUM_OK, NUM_ERR_BAD_INPUT, NUM_ERR_NO_SOLUTION,
            NUM_ERR_INFINITE, NUM_ERR_SINGULAR,
            (Num_Status)99   /* unknown value */
        };
        for (auto c : codes)
            if (num_status_str(c) == nullptr) { e = "returned NULL"; return false; }
        return true;
    });
}

/* ── Entry point ─────────────────────────────────────────────────────────── */

int main()
{
    std::cout << "matrix_ops — test suite\n";
    std::cout << "=======================\n\n";

    test_swap_basic();
    test_swap_idempotent();
    test_scale_basic();
    test_scale_by_one();
    test_scale_inverse();
    test_axpy_basic();
    test_axpy_zero_scalar();
    test_axpy_negation();
    test_dot_basic();
    test_dot_orthogonal();
    test_dot_self_is_norm_sq();
    test_norm_inf_basic();
    test_norm_inf_zero();
    test_mat_vec_identity();
    test_mat_vec_known();
    test_mat_vec_zero_matrix();
    test_status_str_never_null();

    std::cout << "\n=======================\n";
    std::cout << g_pass << " passed, " << g_fail << " failed / "
              << g_pass + g_fail << " total\n";
    return (g_fail == 0) ? 0 : 1;
}
