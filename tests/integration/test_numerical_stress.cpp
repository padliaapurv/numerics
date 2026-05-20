/*
 * test_numerical_stress.cpp
 *
 * Stress tests covering:
 *   - Numerical stability (ill-conditioned, nearly-singular, Hilbert matrices)
 *   - Known instability cases and graceful degradation
 *   - Limits of performance (max dimension NUM_MAX_N)
 *   - Pathological right-hand sides (huge, tiny, zero b)
 *   - Permutation and scaling sensitivity
 *   - Residual verification: ||Ax - b||_inf after solve
 *   - Backward-error bounds
 *
 * Timing note: uses clock() for coarse throughput measurement.
 * Not a hard real-time benchmark — run on a quiet system for stable numbers.
 */

#include "gauss_jordan.h"
#include "mat.h"
#include "vec.h"
#include "numerics.h"
#include <iostream>
#include <cmath>
#include <cstring>
#include <ctime>
#include <string>
#include <functional>

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

static bool near(double a, double b, double tol = 1e-6) { return std::fabs(a-b) < tol; }

/* ── Helpers ─────────────────────────────────────────────────────────────── */

static void zero_A(double A[NUM_MAX_N][NUM_MAX_N])
{
    std::memset(A, 0, sizeof(double) * NUM_MAX_N * NUM_MAX_N);
}

/* Compute residual r = A*x - b, return ||r||_inf. */
static double residual_inf(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double x[NUM_MAX_N],
    const double b[NUM_MAX_N],
    int n
) {
    double y[NUM_MAX_N] = {};
    num_mat_vec(A, x, y, n);
    num_vec_axpy(y, b, -1.0, n);
    return num_vec_norm_inf(y, n);
}

/* Build a Hilbert matrix: H[i][j] = 1/(i+j+1). */
static void hilbert(double A[NUM_MAX_N][NUM_MAX_N], int n)
{
    zero_A(A);
    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            A[i][j] = 1.0 / (double)(i + j + 1);
}

/* Build a random-ish diagonally-dominant matrix (deterministic). */
static void diag_dominant(double A[NUM_MAX_N][NUM_MAX_N], int n, double off = 0.1)
{
    zero_A(A);
    for (int i = 0; i < n; ++i) {
        double row_sum = 0.0;
        for (int j = 0; j < n; ++j) {
            if (i != j) {
                A[i][j] = off * (double)((i * 7 + j * 13) % 5 - 2);
                row_sum += std::fabs(A[i][j]);
            }
        }
        A[i][i] = row_sum + 1.0;
    }
}

/* ── Stability tests ─────────────────────────────────────────────────────── */

static void test_hilbert_3x3()
{
    run("stability: 3×3 Hilbert matrix (cond~500) residual < 1e-10", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        hilbert(A, 3);
        /* b = H * [1,1,1]^T */
        const double ones[NUM_MAX_N] = {1,1,1};
        num_mat_vec(A, ones, b, 3);
        auto s = gauss_jordan_solve(A, b, x, 3);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, 3);
        if (res > 1e-10) { e = "residual " + std::to_string(res) + " > 1e-10"; return false; }
        return true;
    });
}

static void test_hilbert_4x4()
{
    run("stability: 4×4 Hilbert matrix (cond~1.4e4) residual < 1e-8", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        hilbert(A, 4);
        const double ones[NUM_MAX_N] = {1,1,1,1};
        num_mat_vec(A, ones, b, 4);
        auto s = gauss_jordan_solve(A, b, x, 4);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, 4);
        if (res > 1e-8) { e = "residual " + std::to_string(res) + " > 1e-8"; return false; }
        return true;
    });
}

static void test_hilbert_6x6_graceful()
{
    /*
     * 6×6 Hilbert (cond~1.5e7) is still solvable but residual loosens to ~1e-4.
     * We accept either a successful solve with bounded residual OR a non-OK status.
     * What we must NOT see is NUM_OK with a garbage solution (residual > 1).
     */
    run("stability: 6×6 Hilbert — no catastrophic result", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        hilbert(A, 6);
        const double ones[NUM_MAX_N] = {1,1,1,1,1,1};
        num_mat_vec(A, ones, b, 6);
        /* preserve A for residual check — gauss_jordan does not modify A */
        double Asave[NUM_MAX_N][NUM_MAX_N];
        std::memcpy(Asave, A, sizeof(A));
        auto s = gauss_jordan_solve(A, b, x, 6);
        if (s == NUM_OK) {
            double res = residual_inf(Asave, x, b, 6);
            if (res > 1.0) { e = "NUM_OK but residual " + std::to_string(res); return false; }
        }
        return true;
    });
}

static void test_diag_dominant_8x8()
{
    run("stability: 8×8 diag-dominant (well-conditioned) residual < 1e-10", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        diag_dominant(A, 8);
        const double exact[NUM_MAX_N] = {1,2,3,4,5,6,7,8};
        num_mat_vec(A, exact, b, 8);
        auto s = gauss_jordan_solve(A, b, x, 8);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, 8);
        if (res > 1e-10) { e = "residual " + std::to_string(res); return false; }
        return true;
    });
}

static void test_max_dimension()
{
    run("performance: max dimension " + std::to_string(NUM_MAX_N) + "×" +
        std::to_string(NUM_MAX_N) + " diag-dominant", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        diag_dominant(A, NUM_MAX_N);
        double exact[NUM_MAX_N];
        for (int i = 0; i < NUM_MAX_N; ++i) exact[i] = (double)(i + 1);
        num_mat_vec(A, exact, b, NUM_MAX_N);
        auto s = gauss_jordan_solve(A, b, x, NUM_MAX_N);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, NUM_MAX_N);
        if (res > 1e-8) { e = "residual " + std::to_string(res); return false; }
        return true;
    });
}

static void test_throughput()
{
    /*
     * Solve 1000 diag-dominant 16×16 systems and report throughput.
     * No pass/fail criterion — just prints a timing line.
     */
    const int REPS = 1000;
    double A[NUM_MAX_N][NUM_MAX_N], b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
    diag_dominant(A, NUM_MAX_N);
    for (int i = 0; i < NUM_MAX_N; ++i) b[i] = (double)(i + 1);

    std::clock_t t0 = std::clock();
    for (int k = 0; k < REPS; ++k) {
        gauss_jordan_solve(A, b, x, NUM_MAX_N);
    }
    std::clock_t t1 = std::clock();
    double ms = 1000.0 * (double)(t1 - t0) / CLOCKS_PER_SEC;
    std::cout << "  [INFO]  throughput: " << REPS << " solves of " << NUM_MAX_N << "×"
              << NUM_MAX_N << " in " << ms << " ms  ("
              << (ms / REPS * 1000.0) << " µs/solve)\n";
    ++g_pass;
}

/* ── Pathological RHS ────────────────────────────────────────────────────── */

static void test_large_rhs()
{
    run("edge: b entries ~ 1e14 (scale sensitivity)", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        A[0][0]=2; A[0][1]=1;
        A[1][0]=1; A[1][1]=3;
        b[0] = 1e14; b[1] = 2e14;
        auto s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, 2);
        /* Relative residual: res / ||b||_inf < 1e-10 */
        double rel = res / 1e14;
        if (rel > 1e-10) { e = "rel residual " + std::to_string(rel); return false; }
        return true;
    });
}

static void test_tiny_rhs()
{
    run("edge: b entries ~ 1e-14 (scale sensitivity)", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        A[0][0]=2; A[0][1]=1;
        A[1][0]=1; A[1][1]=3;
        b[0] = 1e-14; b[1] = 2e-14;
        auto s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, 2);
        double rel = res / 1e-14;
        if (rel > 1e-6) { e = "rel residual " + std::to_string(rel); return false; }
        return true;
    });
}

static void test_zero_rhs()
{
    run("edge: b=0 → trivial solution x=0", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        A[0][0]=3; A[0][1]=1;
        A[1][0]=2; A[1][1]=5;
        auto s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        if (std::fabs(x[0]) > 1e-12 || std::fabs(x[1]) > 1e-12)
            { e = "non-zero x for zero b"; return false; }
        return true;
    });
}

/* ── Known instability cases ─────────────────────────────────────────────── */

static void test_all_zeros_matrix()
{
    run("instability: all-zero A with non-zero b → NUM_ERR_NO_SOLUTION", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        b[0] = 1.0;
        auto s = gauss_jordan_solve(A, b, x, 3);
        if (s != NUM_ERR_NO_SOLUTION) { e = num_status_str(s); return false; }
        return true;
    });
}

static void test_all_zeros_system()
{
    run("instability: all-zero A and b → NUM_ERR_INFINITE", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        auto s = gauss_jordan_solve(A, b, x, 3);
        if (s != NUM_ERR_INFINITE) { e = num_status_str(s); return false; }
        return true;
    });
}

static void test_duplicate_rows_inconsistent()
{
    run("instability: duplicate rows with different RHS → NUM_ERR_NO_SOLUTION", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        A[0][0]=1; A[0][1]=2; b[0]=3;
        A[1][0]=1; A[1][1]=2; b[1]=7;  /* same LHS, different RHS */
        auto s = gauss_jordan_solve(A, b, x, 2);
        if (s != NUM_ERR_NO_SOLUTION) { e = num_status_str(s); return false; }
        return true;
    });
}

static void test_rank_deficient_underdetermined()
{
    run("instability: rank-1 3×3  →  NUM_ERR_INFINITE or NUM_ERR_NO_SOLUTION", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        /* All rows proportional: rank = 1. */
        A[0][0]=1; A[0][1]=2; A[0][2]=3;
        A[1][0]=2; A[1][1]=4; A[1][2]=6;
        A[2][0]=3; A[2][1]=6; A[2][2]=9;
        b[0]=6; b[1]=12; b[2]=18;   /* consistent: b in column space */
        auto s = gauss_jordan_solve(A, b, x, 3);
        if (s != NUM_ERR_INFINITE && s != NUM_ERR_NO_SOLUTION)
            { e = "expected INFINITE or NO_SOLUTION, got " + std::string(num_status_str(s)); return false; }
        return true;
    });
}

static void test_permutation_sensitivity()
{
    /*
     * The same system solved with rows/cols permuted must give the same solution.
     * Tests that partial pivoting is permutation-insensitive at the solution level.
     */
    run("stability: row-permuted system gives same solution", [](std::string& e) {
        const double Af[]  = {2,1,-1, -3,-1,2, -2,1,2};
        const double bv[]  = {8,-11,-3};
        const double Afp[] = {-3,-1,2, -2,1,2, 2,1,-1};  /* rows 0,1,2 → 1,2,0 */
        const double bvp[] = {-11,-3,8};

        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        double Ap[NUM_MAX_N][NUM_MAX_N]={}, bp[NUM_MAX_N]={}, xp[NUM_MAX_N]={};

        std::memset(A,0,sizeof(A)); std::memset(Ap,0,sizeof(Ap));
        for(int i=0;i<3;++i) for(int j=0;j<3;++j) { A[i][j]=Af[i*3+j]; Ap[i][j]=Afp[i*3+j]; }
        for(int i=0;i<3;++i) { b[i]=bv[i]; bp[i]=bvp[i]; }

        gauss_jordan_solve(A, b, x, 3);
        gauss_jordan_solve(Ap, bp, xp, 3);

        for (int i = 0; i < 3; ++i)
            if (!near(x[i], xp[i], 1e-8))
                { e = "solutions differ at index " + std::to_string(i); return false; }
        return true;
    });
}

/* ── Backward-error check ────────────────────────────────────────────────── */

static void test_backward_error_diag()
{
    run("backward-error: ||Ax-b||_inf / ||b||_inf < 1e-12 for 10×10 diagonal system",
        [](std::string& e)
    {
        const int n = 10;
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        for (int i = 0; i < n; ++i) {
            A[i][i] = (double)(i + 1);
            b[i]    = (double)(i + 1) * (double)(i + 2);
        }
        auto s = gauss_jordan_solve(A, b, x, n);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, n);
        double norm_b = num_vec_norm_inf(b, n);
        double rel = res / norm_b;
        if (rel > 1e-12) { e = "rel backward error " + std::to_string(rel); return false; }
        return true;
    });
}

static void test_backward_error_dense()
{
    run("backward-error: ||Ax-b||_inf / ||b||_inf < 1e-10 for 12×12 dense diag-dominant",
        [](std::string& e)
    {
        const int n = 12;
        double A[NUM_MAX_N][NUM_MAX_N]={}, b[NUM_MAX_N]={}, x[NUM_MAX_N]={};
        diag_dominant(A, n, 0.3);
        double exact[NUM_MAX_N];
        for (int i = 0; i < n; ++i) exact[i] = (double)(i % 5 + 1);
        num_mat_vec(A, exact, b, n);
        auto s = gauss_jordan_solve(A, b, x, n);
        if (s != NUM_OK) { e = num_status_str(s); return false; }
        double res = residual_inf(A, x, b, n);
        double norm_b = num_vec_norm_inf(b, n);
        double rel = res / norm_b;
        if (rel > 1e-10) { e = "rel backward error " + std::to_string(rel); return false; }
        return true;
    });
}

/* ── Entry point ─────────────────────────────────────────────────────────── */

int main()
{
    std::cout << "numerical stress & stability tests\n";
    std::cout << "===================================\n\n";

    test_hilbert_3x3();
    test_hilbert_4x4();
    test_hilbert_6x6_graceful();
    test_diag_dominant_8x8();
    test_max_dimension();
    test_throughput();
    test_large_rhs();
    test_tiny_rhs();
    test_zero_rhs();
    test_all_zeros_matrix();
    test_all_zeros_system();
    test_duplicate_rows_inconsistent();
    test_rank_deficient_underdetermined();
    test_permutation_sensitivity();
    test_backward_error_diag();
    test_backward_error_dense();

    std::cout << "\n===================================\n";
    std::cout << g_pass << " passed, " << g_fail << " failed / "
              << g_pass + g_fail << " total\n";
    return (g_fail == 0) ? 0 : 1;
}
