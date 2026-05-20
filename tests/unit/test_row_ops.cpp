#include "framework.h"
#include "row_ops.h"

static void test_row_swap()
{
    run("row_swap: contents exchanged", [](std::string& e) {
        double r1[] = {1,2,3}, r2[] = {4,5,6};
        num_row_swap(r1, r2, 3);
        if (!near_tol(r1[0],4)||!near_tol(r2[0],1)) { e = "wrong"; return false; }
        return true;
    });
}

static void test_row_swap_double()
{
    run("row_swap: double-swap restores original", [](std::string& e) {
        double r1[] = {7,-2,0.5}, r2[] = {3,1,9.0};
        double o1[3], o2[3];
        std::memcpy(o1,r1,sizeof(r1)); std::memcpy(o2,r2,sizeof(r2));
        num_row_swap(r1,r2,3); num_row_swap(r1,r2,3);
        for (int i=0;i<3;++i)
            if (!near_tol(r1[i],o1[i])||!near_tol(r2[i],o2[i]))
                { e="not restored"; return false; }
        return true;
    });
}

static void test_row_scale_basic()
{
    run("row_scale: each element multiplied", [](std::string& e) {
        double r[] = {1,2,4};
        num_row_scale(r, 0.5, 3);
        if (!near_tol(r[0],0.5)||!near_tol(r[1],1.0)||!near_tol(r[2],2.0))
            { e="wrong"; return false; }
        return true;
    });
}

static void test_row_scale_by_one()
{
    run("row_scale: by 1 is identity", [](std::string& e) {
        double r[] = {3.14,-2.71,0.0};
        double orig[3]; std::memcpy(orig,r,sizeof(r));
        num_row_scale(r, 1.0, 3);
        for (int i=0;i<3;++i)
            if (!near_tol(r[i],orig[i])) { e="changed"; return false; }
        return true;
    });
}

static void test_row_scale_inverse()
{
    run("row_scale: scale then inverse restores", [](std::string& e) {
        double r[] = {6,-3,9};
        num_row_scale(r, 2.0, 3); num_row_scale(r, 0.5, 3);
        if (!near_tol(r[0],6)||!near_tol(r[1],-3)||!near_tol(r[2],9))
            { e="not restored"; return false; }
        return true;
    });
}

static void test_row_axpy_basic()
{
    run("row_axpy: dst += 2 * src", [](std::string& e) {
        double dst[] = {1,2,3};
        const double src[] = {10,20,30};
        num_row_axpy(dst, src, 2.0, 3);
        if (!near_tol(dst[0],21)||!near_tol(dst[1],42)||!near_tol(dst[2],63))
            { e="wrong"; return false; }
        return true;
    });
}

static void test_row_axpy_zero()
{
    run("row_axpy: scalar=0 leaves dst unchanged", [](std::string& e) {
        double dst[] = {5,6,7};
        const double src[] = {100,200,300};
        double orig[3]; std::memcpy(orig,dst,sizeof(dst));
        num_row_axpy(dst, src, 0.0, 3);
        for (int i=0;i<3;++i)
            if (!near_tol(dst[i],orig[i])) { e="changed"; return false; }
        return true;
    });
}

static void test_row_axpy_negation()
{
    run("row_axpy: dst -= src zeroes identical rows", [](std::string& e) {
        double dst[] = {3,-1,7};
        const double src[] = {3,-1,7};
        num_row_axpy(dst, src, -1.0, 3);
        for (int i=0;i<3;++i)
            if (!near_tol(dst[i],0.0)) { e="not zero"; return false; }
        return true;
    });
}

/* Alias tests — confirm old names still resolve. */

static void test_aliases_link()
{
    run("aliases: num_swap_rows / num_scale_row / num_axpy_row exist and work", [](std::string& e) {
        double r1[] = {1,0}, r2[] = {0,1};
        num_swap_rows(r1, r2, 2);
        if (!near_tol(r1[0],0.0)) { e="swap alias wrong"; return false; }
        num_scale_row(r1, 3.0, 2);
        if (!near_tol(r1[1],3.0)) { e="scale alias wrong"; return false; }
        double dst[] = {1,1};
        const double src[] = {1,1};
        num_axpy_row(dst, src, -1.0, 2);
        if (!near_tol(dst[0],0.0)) { e="axpy alias wrong"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "row_ops — unit tests\n";
    std::cout << "====================\n\n";
    test_row_swap();
    test_row_swap_double();
    test_row_scale_basic();
    test_row_scale_by_one();
    test_row_scale_inverse();
    test_row_axpy_basic();
    test_row_axpy_zero();
    test_row_axpy_negation();
    test_aliases_link();
    return suite_result("row_ops");
}
