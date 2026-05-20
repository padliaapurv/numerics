#include "framework.h"
#include "mat.h"
#include "numerics.h"

static void test_mat_vec_identity()
{
    run("mat_vec: I*x = x", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        for (int i = 0; i < 3; ++i) A[i][i] = 1.0;
        const double x[] = {7,-2,4};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(y[i], x[i])) { e="I*x != x"; return false; }
        return true;
    });
}

static void test_mat_vec_known()
{
    run("mat_vec: [[1,2],[3,4]]*[1,1] = [3,7]", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        A[0][0]=1; A[0][1]=2; A[1][0]=3; A[1][1]=4;
        const double x[] = {1,1};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 2);
        if (!near_tol(y[0],3.0)||!near_tol(y[1],7.0)) { e="wrong"; return false; }
        return true;
    });
}

static void test_mat_vec_zero()
{
    run("mat_vec: 0*x = 0", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        const double x[] = {5,-3,1};
        double y[NUM_MAX_N] = {};
        num_mat_vec(A, x, y, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(y[i],0.0)) { e="0*x != 0"; return false; }
        return true;
    });
}

static void test_mat_zero()
{
    run("mat_zero: all active entries become 0", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N];
        for (int i = 0; i < NUM_MAX_N; ++i)
            for (int j = 0; j < NUM_MAX_N; ++j) A[i][j] = (double)(i * 16 + j + 1);
        num_mat_zero(A, 4);
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j)
                if (!near_tol(A[i][j], 0.0)) { e="non-zero entry"; return false; }
        return true;
    });
}

static void test_mat_identity()
{
    run("mat_identity: diagonal 1, off-diagonal 0", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        num_mat_identity(A, 4);
        for (int i = 0; i < 4; ++i)
            for (int j = 0; j < 4; ++j) {
                double expected = (i == j) ? 1.0 : 0.0;
                if (!near_tol(A[i][j], expected)) { e="wrong entry"; return false; }
            }
        return true;
    });
}

static void test_mat_copy()
{
    run("mat_copy: dst matches src after copy", [](std::string& e) {
        double src[NUM_MAX_N][NUM_MAX_N] = {};
        double dst[NUM_MAX_N][NUM_MAX_N] = {};
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j) src[i][j] = (double)(i * 3 + j + 1);
        num_mat_copy(dst, src, 3);
        for (int i = 0; i < 3; ++i)
            for (int j = 0; j < 3; ++j)
                if (!near_tol(dst[i][j], src[i][j])) { e="mismatch"; return false; }
        return true;
    });
}

static void test_mat_copy_independence()
{
    run("mat_copy: modifying dst does not affect src", [](std::string& e) {
        double src[NUM_MAX_N][NUM_MAX_N] = {};
        double dst[NUM_MAX_N][NUM_MAX_N] = {};
        src[0][0] = 5.0;
        num_mat_copy(dst, src, 2);
        dst[0][0] = 99.0;
        if (!near_tol(src[0][0], 5.0)) { e="src modified"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "mat — unit tests\n";
    std::cout << "================\n\n";
    test_mat_vec_identity();
    test_mat_vec_known();
    test_mat_vec_zero();
    test_mat_zero();
    test_mat_identity();
    test_mat_copy();
    test_mat_copy_independence();
    return suite_result("mat");
}
