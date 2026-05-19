/*
 * test_gauss_jordan.cpp  — NOT safety-critical; STL/iostream permitted.
 *
 * Mimics how flight or sensor software would call gauss_jordan_solve:
 * stage inputs into fixed-size arrays, invoke the solver, inspect the
 * Num_Status return code, and read x[] only on NUM_OK.
 */

#include "gauss_jordan.h"
#include "numerics.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <functional>
#include <vector>

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

static bool near(double a, double b, double tol = 1e-6)
{
    return std::fabs(a - b) < tol;
}

/* ── Staging helpers (mirror embedded usage pattern) ─────────────────────── */

static void load_A(double dst[NUM_MAX_N][NUM_MAX_N], const double* src, int n)
{
    std::memset(dst, 0, sizeof(double) * NUM_MAX_N * NUM_MAX_N);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            dst[i][j] = src[i * n + j];
}

static void load_b(double dst[NUM_MAX_N], const double* src, int n)
{
    std::memset(dst, 0, sizeof(double) * NUM_MAX_N);
    for (int i = 0; i < n; ++i) dst[i] = src[i];
}

/* ── Tests ───────────────────────────────────────────────────────────────── */

static void test_1x1()
{
    run("1x1  5x = 15  →  x = 3", [](std::string& e) {
        const double Af[] = {5};  const double bv[] = {15};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 1);  load_b(b, bv, 1);
        Num_Status s = gauss_jordan_solve(A, b, x, 1);
        if (s != NUM_OK)       { e = num_status_str(s); return false; }
        if (!near(x[0], 3.0)) { e = "x[0] != 3"; return false; }
        return true;
    });
}

static void test_2x2_unique()
{
    run("2x2  3x+2y=12, x-y=1  →  x=14/5, y=9/5", [](std::string& e) {
        const double Af[] = {3, 2, 1, -1};  const double bv[] = {12, 1};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 2);  load_b(b, bv, 2);
        Num_Status s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        if (!near(x[0], 14.0/5.0) || !near(x[1], 9.0/5.0)) {
            e = "solution mismatch"; return false;
        }
        return true;
    });
}

static void test_3x3_unique()
{
    run("3x3  textbook case  →  x=2, y=3, z=-1", [](std::string& e) {
        const double Af[] = {2,1,-1, -3,-1,2, -2,1,2};
        const double bv[] = {8, -11, -3};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 3);  load_b(b, bv, 3);
        Num_Status s = gauss_jordan_solve(A, b, x, 3);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        if (!near(x[0],2) || !near(x[1],3) || !near(x[2],-1)) {
            e = "solution mismatch"; return false;
        }
        return true;
    });
}

static void test_4x4_unique()
{
    run("4x4  tridiagonal  →  x={1,2,2,1}", [](std::string& e) {
        const double Af[] = {
             4,-1, 0, 0,
            -1, 4,-1, 0,
             0,-1, 4,-1,
             0, 0,-1, 4
        };
        const double bv[] = {4*1-2, -1+4*2-2, -2+4*2-1, -2+4*1};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 4);  load_b(b, bv, 4);
        Num_Status s = gauss_jordan_solve(A, b, x, 4);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        const double ex[] = {1,2,2,1};
        for (int i = 0; i < 4; ++i)
            if (!near(x[i], ex[i])) { e = "solution mismatch"; return false; }
        return true;
    });
}

static void test_identity()
{
    run("4x4  identity  →  x = b", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        double b[NUM_MAX_N] = {}, x[NUM_MAX_N] = {};
        const double bv[] = {1,2,3,4};
        for (int i = 0; i < 4; ++i) A[i][i] = 1.0;
        load_b(b, bv, 4);
        Num_Status s = gauss_jordan_solve(A, b, x, 4);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        for (int i = 0; i < 4; ++i)
            if (!near(x[i], bv[i])) { e = "identity mismatch"; return false; }
        return true;
    });
}

static void test_no_solution()
{
    run("2x2  x+y=1, x+y=2  →  NUM_ERR_NO_SOLUTION", [](std::string& e) {
        const double Af[] = {1,1, 1,1};  const double bv[] = {1, 2};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 2);  load_b(b, bv, 2);
        Num_Status s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_ERR_NO_SOLUTION) {
            e = std::string("got: ") + num_status_str(s); return false;
        }
        return true;
    });
}

static void test_infinite()
{
    run("2x2  x+y=3, 2x+2y=6  →  NUM_ERR_INFINITE", [](std::string& e) {
        const double Af[] = {1,1, 2,2};  const double bv[] = {3, 6};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 2);  load_b(b, bv, 2);
        Num_Status s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_ERR_INFINITE) {
            e = std::string("got: ") + num_status_str(s); return false;
        }
        return true;
    });
}

static void test_near_singular()
{
    run("2x2  near-singular  →  no catastrophic result", [](std::string& e) {
        const double Af[] = {1.0, 2.0, 1.0, 2.0 + 1e-15};
        const double bv[] = {3.0, 3.0};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 2);  load_b(b, bv, 2);
        Num_Status s = gauss_jordan_solve(A, b, x, 2);
        if (s == NUM_OK && (std::fabs(x[0]) > 1e8 || std::fabs(x[1]) > 1e8)) {
            e = "returned NUM_OK with exploded solution"; return false;
        }
        return true;
    });
}

static void test_bad_input_zero()
{
    run("n=0  →  NUM_ERR_BAD_INPUT", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        Num_Status s = gauss_jordan_solve(A, b, x, 0);
        if (s != NUM_ERR_BAD_INPUT) { e = num_status_str(s); return false; }
        return true;
    });
}

static void test_bad_input_overflow()
{
    run("n > NUM_MAX_N  →  NUM_ERR_BAD_INPUT", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        Num_Status s = gauss_jordan_solve(A, b, x, NUM_MAX_N + 1);
        if (s != NUM_ERR_BAD_INPUT) { e = num_status_str(s); return false; }
        return true;
    });
}

static void test_inputs_not_modified()
{
    run("A and b are not modified by the solver", [](std::string& e) {
        const double Af[] = {2,1,-1, -3,-1,2, -2,1,2};
        const double bv[] = {8, -11, -3};
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N], x[NUM_MAX_N];
        load_A(A, Af, 3);  load_b(b, bv, 3);
        double Ac[NUM_MAX_N][NUM_MAX_N], bc[NUM_MAX_N];
        std::memcpy(Ac, A, sizeof(A));
        std::memcpy(bc, b, sizeof(b));
        gauss_jordan_solve(A, b, x, 3);
        if (std::memcmp(A, Ac, sizeof(A)) != 0) { e = "A was modified"; return false; }
        if (std::memcmp(b, bc, sizeof(b)) != 0) { e = "b was modified"; return false; }
        return true;
    });
}

/* ── Entry point ─────────────────────────────────────────────────────────── */

int main()
{
    std::cout << "gauss_jordan_solve — test suite\n";
    std::cout << "================================\n\n";

    test_1x1();
    test_2x2_unique();
    test_3x3_unique();
    test_4x4_unique();
    test_identity();
    test_no_solution();
    test_infinite();
    test_near_singular();
    test_bad_input_zero();
    test_bad_input_overflow();
    test_inputs_not_modified();

    std::cout << "\n================================\n";
    std::cout << g_pass << " passed, " << g_fail << " failed / "
              << g_pass + g_fail << " total\n";
    return (g_fail == 0) ? 0 : 1;
}
