#include "framework.h"
#include "pivot.h"
#include "numerics.h"
#include <cstring>

static double g_aug[NUM_MAX_N][NUM_MAX_N + 1];

static void clear() { std::memset(g_aug, 0, sizeof(g_aug)); }

static void test_finds_max()
{
    run("find_max_row: returns row with largest |value|", [](std::string& e) {
        clear();
        g_aug[0][0] = 1.0;
        g_aug[1][0] = 5.0;
        g_aug[2][0] = -3.0;
        int r = num_find_max_row(g_aug, 0, 0, 3);
        if (r != 1) { e = "expected row 1"; return false; }
        return true;
    });
}

static void test_skips_above_start()
{
    run("find_max_row: ignores rows above start_row", [](std::string& e) {
        clear();
        g_aug[0][0] = 100.0;   /* above start — must be ignored */
        g_aug[1][0] = 2.0;
        g_aug[2][0] = 3.0;
        int r = num_find_max_row(g_aug, 1, 0, 3);
        if (r != 2) { e = "expected row 2"; return false; }
        return true;
    });
}

static void test_all_zero_returns_neg1()
{
    run("find_max_row: all near-zero → -1", [](std::string& e) {
        clear();
        g_aug[0][0] = 0.0;
        g_aug[1][0] = 1e-12;
        int r = num_find_max_row(g_aug, 0, 0, 2);
        if (r != -1) { e = "expected -1"; return false; }
        return true;
    });
}

static void test_single_row()
{
    run("find_max_row: single usable row → that row", [](std::string& e) {
        clear();
        g_aug[0][2] = 7.0;
        int r = num_find_max_row(g_aug, 0, 2, 1);
        if (r != 0) { e = "expected row 0"; return false; }
        return true;
    });
}

static void test_negative_dominates()
{
    run("find_max_row: large negative has larger abs than small positive", [](std::string& e) {
        clear();
        g_aug[0][1] = 2.0;
        g_aug[1][1] = -9.0;
        g_aug[2][1] = 3.0;
        int r = num_find_max_row(g_aug, 0, 1, 3);
        if (r != 1) { e = "expected row 1 (-9)"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "pivot — unit tests\n";
    std::cout << "==================\n\n";
    test_finds_max();
    test_skips_above_start();
    test_all_zero_returns_neg1();
    test_single_row();
    test_negative_dominates();
    return suite_result("pivot");
}
