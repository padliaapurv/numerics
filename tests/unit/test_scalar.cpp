#include "framework.h"
#include "scalar.h"

static void test_near_zero_below()
{
    run("near_zero: value below epsilon → 1", [](std::string& e) {
        if (!num_near_zero(0.0))        { e = "0.0 not near-zero"; return false; }
        if (!num_near_zero(1e-11))      { e = "1e-11 not near-zero"; return false; }
        if (!num_near_zero(-1e-11))     { e = "-1e-11 not near-zero"; return false; }
        return true;
    });
}

static void test_near_zero_above()
{
    run("near_zero: value at/above epsilon → 0", [](std::string& e) {
        if (num_near_zero(NUM_EPSILON))    { e = "epsilon flagged as zero"; return false; }
        if (num_near_zero(1.0))            { e = "1.0 flagged as zero"; return false; }
        if (num_near_zero(-1.0))           { e = "-1.0 flagged as zero"; return false; }
        if (num_near_zero(1e-9))           { e = "1e-9 flagged as zero"; return false; }
        return true;
    });
}

static void test_near_zero_boundary()
{
    run("near_zero: just below epsilon is zero, just above is not", [](std::string& e) {
        double below = NUM_EPSILON * 0.5;
        double above = NUM_EPSILON * 1.5;
        if (!num_near_zero(below)) { e = "below epsilon not flagged"; return false; }
        if ( num_near_zero(above)) { e = "above epsilon flagged"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "scalar — unit tests\n";
    std::cout << "===================\n\n";
    test_near_zero_below();
    test_near_zero_above();
    test_near_zero_boundary();
    return suite_result("scalar");
}
