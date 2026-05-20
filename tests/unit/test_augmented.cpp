#include "framework.h"
#include "augmented.h"
#include "numerics.h"
#include <cstring>

static void test_build_copies_correctly()
{
    run("aug_build: A and b land in correct columns", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        double b[NUM_MAX_N] = {};
        double aug[NUM_MAX_N][NUM_MAX_N + 1] = {};
        A[0][0]=1; A[0][1]=2;
        A[1][0]=3; A[1][1]=4;
        b[0]=5; b[1]=6;
        num_aug_build(aug, A, b, 2);
        if (!near_tol(aug[0][0],1)||!near_tol(aug[0][1],2)||!near_tol(aug[0][2],5))
            { e="row 0 wrong"; return false; }
        if (!near_tol(aug[1][0],3)||!near_tol(aug[1][1],4)||!near_tol(aug[1][2],6))
            { e="row 1 wrong"; return false; }
        return true;
    });
}

static void test_build_pads_zero()
{
    run("aug_build: padding beyond column n is zero", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        double b[NUM_MAX_N] = {};
        double aug[NUM_MAX_N][NUM_MAX_N + 1];
        std::memset(aug, 0xFF, sizeof(aug));  /* poison */
        A[0][0] = 1.0; b[0] = 2.0;
        num_aug_build(aug, A, b, 1);
        for (int j = 2; j <= NUM_MAX_N; ++j)
            if (!near_tol(aug[0][j], 0.0)) { e="padding non-zero"; return false; }
        return true;
    });
}

static void test_inconsistent_detects()
{
    run("aug_is_inconsistent: row [0..0 | nonzero] → 1", [](std::string& e) {
        double aug[NUM_MAX_N][NUM_MAX_N + 1] = {};
        aug[1][2] = 5.0;  /* row 1: LHS zeros, RHS=5 for n=2 */
        if (!num_aug_is_inconsistent(aug, 2)) { e="not flagged"; return false; }
        return true;
    });
}

static void test_inconsistent_clean()
{
    run("aug_is_inconsistent: consistent system → 0", [](std::string& e) {
        double aug[NUM_MAX_N][NUM_MAX_N + 1] = {};
        aug[0][0]=1; aug[0][2]=3;
        aug[1][1]=1; aug[1][2]=4;
        if (num_aug_is_inconsistent(aug, 2)) { e="false positive"; return false; }
        return true;
    });
}

static void test_extract_copies_rhs()
{
    run("aug_extract: x[i] = aug[i][n]", [](std::string& e) {
        double aug[NUM_MAX_N][NUM_MAX_N + 1] = {};
        aug[0][3] = 7.0;
        aug[1][3] = -2.0;
        aug[2][3] = 5.0;
        double x[NUM_MAX_N] = {};
        num_aug_extract(x, aug, 3);
        if (!near_tol(x[0],7.0)||!near_tol(x[1],-2.0)||!near_tol(x[2],5.0))
            { e="wrong values"; return false; }
        return true;
    });
}

static void test_build_does_not_touch_A()
{
    run("aug_build: A is not modified", [](std::string& e) {
        double A[NUM_MAX_N][NUM_MAX_N] = {};
        double b[NUM_MAX_N] = {};
        double aug[NUM_MAX_N][NUM_MAX_N + 1] = {};
        A[0][0]=3.0; A[1][1]=4.0; b[0]=1.0; b[1]=2.0;
        double Acopy[NUM_MAX_N][NUM_MAX_N];
        std::memcpy(Acopy, A, sizeof(A));
        num_aug_build(aug, A, b, 2);
        if (std::memcmp(A, Acopy, sizeof(A)) != 0) { e="A modified"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "augmented — unit tests\n";
    std::cout << "======================\n\n";
    test_build_copies_correctly();
    test_build_pads_zero();
    test_inconsistent_detects();
    test_inconsistent_clean();
    test_extract_copies_rhs();
    test_build_does_not_touch_A();
    return suite_result("augmented");
}
