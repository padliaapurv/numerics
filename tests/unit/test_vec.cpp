#include "framework.h"
#include "vec.h"

/* ── dot ─────────────────────────────────────────────────────────────────── */

static void test_dot_basic()
{
    run("dot: [1,2,3]·[4,5,6] = 32", [](std::string& e) {
        const double a[] = {1,2,3}, b[] = {4,5,6};
        if (!near_tol(num_vec_dot(a, b, 3), 32.0)) { e = "wrong value"; return false; }
        return true;
    });
}

static void test_dot_orthogonal()
{
    run("dot: orthogonal vectors → 0", [](std::string& e) {
        const double a[] = {1,0}, b[] = {0,1};
        if (!near_tol(num_vec_dot(a, b, 2), 0.0)) { e = "not 0"; return false; }
        return true;
    });
}

static void test_dot_self_is_norm_sq()
{
    run("dot: v·v = ||v||²  ([3,4] → 25)", [](std::string& e) {
        const double v[] = {3,4};
        if (!near_tol(num_vec_dot(v, v, 2), 25.0)) { e = "not 25"; return false; }
        return true;
    });
}

/* ── norms ───────────────────────────────────────────────────────────────── */

static void test_norm_inf_basic()
{
    run("norm_inf: max |v[i]|  ([1,-5,3,-2] → 5)", [](std::string& e) {
        const double v[] = {1,-5,3,-2};
        if (!near_tol(num_vec_norm_inf(v, 4), 5.0)) { e = "not 5"; return false; }
        return true;
    });
}

static void test_norm_inf_zero_vec()
{
    run("norm_inf: zero vector → 0", [](std::string& e) {
        const double v[] = {0,0,0};
        if (!near_tol(num_vec_norm_inf(v, 3), 0.0)) { e = "not 0"; return false; }
        return true;
    });
}

static void test_norm_1_basic()
{
    run("norm_1: sum |v[i]|  ([1,-2,3] → 6)", [](std::string& e) {
        const double v[] = {1,-2,3};
        if (!near_tol(num_vec_norm_1(v, 3), 6.0)) { e = "not 6"; return false; }
        return true;
    });
}

static void test_norm_2sq_basic()
{
    run("norm_2sq: [3,4] → 25", [](std::string& e) {
        const double v[] = {3,4};
        if (!near_tol(num_vec_norm_2sq(v, 2), 25.0)) { e = "not 25"; return false; }
        return true;
    });
}

/* ── is_zero ─────────────────────────────────────────────────────────────── */

static void test_is_zero_true()
{
    run("is_zero: all near-zero → 1", [](std::string& e) {
        const double v[] = {0.0, 1e-11, -1e-12};
        if (!num_vec_is_zero(v, 3)) { e = "returned 0"; return false; }
        return true;
    });
}

static void test_is_zero_false()
{
    run("is_zero: one non-zero entry → 0", [](std::string& e) {
        const double v[] = {0.0, 0.0, 1.0};
        if (num_vec_is_zero(v, 3)) { e = "returned 1"; return false; }
        return true;
    });
}

/* ── mutations ───────────────────────────────────────────────────────────── */

static void test_scale_basic()
{
    run("scale: [1,2,4] * 0.5 = [0.5,1,2]", [](std::string& e) {
        double v[] = {1,2,4};
        num_vec_scale(v, 0.5, 3);
        if (!near_tol(v[0],0.5)||!near_tol(v[1],1.0)||!near_tol(v[2],2.0))
            { e = "wrong values"; return false; }
        return true;
    });
}

static void test_scale_by_one()
{
    run("scale: by 1.0 is identity", [](std::string& e) {
        double v[] = {3.14, -2.71, 0.0};
        double orig[3]; std::memcpy(orig, v, sizeof(v));
        num_vec_scale(v, 1.0, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(v[i], orig[i])) { e = "changed"; return false; }
        return true;
    });
}

static void test_axpy_basic()
{
    run("axpy: dst += 2 * src  ([1,2,3] + 2*[10,20,30] = [21,42,63])", [](std::string& e) {
        double dst[] = {1,2,3};
        const double src[] = {10,20,30};
        num_vec_axpy(dst, src, 2.0, 3);
        if (!near_tol(dst[0],21)||!near_tol(dst[1],42)||!near_tol(dst[2],63))
            { e = "wrong"; return false; }
        return true;
    });
}

static void test_axpy_zero_scalar()
{
    run("axpy: scalar=0 leaves dst unchanged", [](std::string& e) {
        double dst[] = {5,6,7};
        const double src[] = {100,200,300};
        double orig[3]; std::memcpy(orig, dst, sizeof(dst));
        num_vec_axpy(dst, src, 0.0, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(dst[i], orig[i])) { e = "changed"; return false; }
        return true;
    });
}

static void test_copy()
{
    run("copy: dst[i] == src[i] after copy", [](std::string& e) {
        const double src[] = {1,-2,3,-4};
        double dst[4] = {};
        num_vec_copy(dst, src, 4);
        for (int i = 0; i < 4; ++i)
            if (!near_tol(dst[i], src[i])) { e = "mismatch"; return false; }
        return true;
    });
}

static void test_zero()
{
    run("zero: all entries become 0", [](std::string& e) {
        double v[] = {1,2,3};
        num_vec_zero(v, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(v[i], 0.0)) { e = "non-zero entry"; return false; }
        return true;
    });
}

static void test_swap()
{
    run("swap: contents exchanged", [](std::string& e) {
        double a[] = {1,2,3}, b[] = {4,5,6};
        num_vec_swap(a, b, 3);
        if (!near_tol(a[0],4)||!near_tol(a[1],5)||!near_tol(a[2],6)) { e = "a wrong"; return false; }
        if (!near_tol(b[0],1)||!near_tol(b[1],2)||!near_tol(b[2],3)) { e = "b wrong"; return false; }
        return true;
    });
}

static void test_swap_idempotent()
{
    run("swap: double-swap restores original", [](std::string& e) {
        double a[] = {7,-2,0.5}, b[] = {3,1,9.0};
        double oa[3], ob[3];
        std::memcpy(oa, a, sizeof(a)); std::memcpy(ob, b, sizeof(b));
        num_vec_swap(a, b, 3); num_vec_swap(a, b, 3);
        for (int i = 0; i < 3; ++i)
            if (!near_tol(a[i],oa[i])||!near_tol(b[i],ob[i])) { e = "not restored"; return false; }
        return true;
    });
}

int main()
{
    std::cout << "vec — unit tests\n";
    std::cout << "================\n\n";
    test_dot_basic();
    test_dot_orthogonal();
    test_dot_self_is_norm_sq();
    test_norm_inf_basic();
    test_norm_inf_zero_vec();
    test_norm_1_basic();
    test_norm_2sq_basic();
    test_is_zero_true();
    test_is_zero_false();
    test_scale_basic();
    test_scale_by_one();
    test_axpy_basic();
    test_axpy_zero_scalar();
    test_copy();
    test_zero();
    test_swap();
    test_swap_idempotent();
    return suite_result("vec");
}
