"""
test_matrix_ops.py — Python-level tests for the matrix_ops bindings.
"""

import sys
import pathlib
import math

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))

import numpy as np
from numerics import matrix_ops

# ── Minimal test framework ────────────────────────────────────────────────────

_pass = _fail = 0

def check(name, fn):
    global _pass, _fail
    try:
        fn()
        print(f"  [PASS]  {name}")
        _pass += 1
    except AssertionError as e:
        print(f"  [FAIL]  {name}\n         {e}")
        _fail += 1
    except Exception as e:
        print(f"  [FAIL]  {name}\n         unexpected: {e}")
        _fail += 1

def allclose(a, b, tol=1e-12):
    return np.allclose(a, b, atol=tol)

# ── swap_rows ─────────────────────────────────────────────────────────────────

def test_swap_basic():
    r1, r2 = matrix_ops.swap_rows([1, 2, 3], [4, 5, 6])
    assert allclose(r1, [4, 5, 6]) and allclose(r2, [1, 2, 3])

def test_swap_idempotent():
    orig1, orig2 = [7.0, -2.0, 0.5], [3.0, 1.0, 9.0]
    r1, r2 = matrix_ops.swap_rows(orig1[:], orig2[:])
    r1, r2 = matrix_ops.swap_rows(r1, r2)
    assert allclose(r1, orig1) and allclose(r2, orig2)

def test_swap_single_element():
    r1, r2 = matrix_ops.swap_rows([42.0], [-7.0])
    assert allclose(r1, [-7.0]) and allclose(r2, [42.0])

# ── scale_row ─────────────────────────────────────────────────────────────────

def test_scale_basic():
    r = matrix_ops.scale_row([1, 2, 4], 0.5)
    assert allclose(r, [0.5, 1.0, 2.0])

def test_scale_identity():
    orig = [3.14, -2.71, 0.0]
    r = matrix_ops.scale_row(orig[:], 1.0)
    assert allclose(r, orig)

def test_scale_zero():
    r = matrix_ops.scale_row([5, -3, 9], 0.0)
    assert allclose(r, [0, 0, 0])

def test_scale_roundtrip():
    r = matrix_ops.scale_row([6, -3, 9], 2.0)
    r = matrix_ops.scale_row(r, 0.5)
    assert allclose(r, [6, -3, 9])

# ── axpy_row ──────────────────────────────────────────────────────────────────

def test_axpy_basic():
    dst = matrix_ops.axpy_row([1, 2, 3], [10, 20, 30], 2.0)
    assert allclose(dst, [21, 42, 63])

def test_axpy_zero_scalar():
    orig = [5.0, 6.0, 7.0]
    dst = matrix_ops.axpy_row(orig[:], [100, 200, 300], 0.0)
    assert allclose(dst, orig)

def test_axpy_self_cancel():
    v = [3.0, -1.0, 7.0]
    dst = matrix_ops.axpy_row(v[:], v[:], -1.0)
    assert allclose(dst, [0, 0, 0])

# ── dot ───────────────────────────────────────────────────────────────────────

def test_dot_known():
    assert math.isclose(matrix_ops.dot([1, 2, 3], [4, 5, 6]), 32.0)

def test_dot_orthogonal():
    assert math.isclose(matrix_ops.dot([1, 0], [0, 1]), 0.0)

def test_dot_self_is_norm_sq():
    assert math.isclose(matrix_ops.dot([3, 4], [3, 4]), 25.0)

def test_dot_agrees_with_numpy():
    a = np.random.default_rng(0).random(8)
    b = np.random.default_rng(1).random(8)
    assert math.isclose(matrix_ops.dot(a, b), float(np.dot(a, b)), rel_tol=1e-12)

# ── norm_inf ──────────────────────────────────────────────────────────────────

def test_norm_inf_basic():
    assert math.isclose(matrix_ops.norm_inf([1, -5, 3, -2]), 5.0)

def test_norm_inf_zero():
    assert math.isclose(matrix_ops.norm_inf([0, 0, 0]), 0.0)

def test_norm_inf_positive():
    assert math.isclose(matrix_ops.norm_inf([2, 2, 2]), 2.0)

# ── mat_vec ───────────────────────────────────────────────────────────────────

def test_mat_vec_identity():
    I = np.eye(4)
    x = [7, -2, 4, 1]
    assert allclose(matrix_ops.mat_vec(I, x), x)

def test_mat_vec_known():
    A = [[1, 2], [3, 4]]
    x = [1, 1]
    assert allclose(matrix_ops.mat_vec(A, x), [3, 7])

def test_mat_vec_zero_matrix():
    assert allclose(matrix_ops.mat_vec(np.zeros((3, 3)), [5, -3, 1]), [0, 0, 0])

def test_mat_vec_agrees_with_numpy():
    rng = np.random.default_rng(42)
    A = rng.random((5, 5))
    x = rng.random(5)
    assert allclose(matrix_ops.mat_vec(A, x), A @ x)

# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("matrix_ops (Python) — test suite")
    print("==================================\n")

    check("swap_rows: basic exchange",                test_swap_basic)
    check("swap_rows: double-swap restores",          test_swap_idempotent)
    check("swap_rows: single element",                test_swap_single_element)
    check("scale_row: halve values",                  test_scale_basic)
    check("scale_row: *1 is identity",                test_scale_identity)
    check("scale_row: *0 zeroes row",                 test_scale_zero)
    check("scale_row: *2 then *0.5 restores",         test_scale_roundtrip)
    check("axpy_row: dst += 2*src",                   test_axpy_basic)
    check("axpy_row: scalar=0 no change",             test_axpy_zero_scalar)
    check("axpy_row: dst -= dst zeroes",              test_axpy_self_cancel)
    check("dot: [1,2,3]·[4,5,6] = 32",               test_dot_known)
    check("dot: orthogonal → 0",                      test_dot_orthogonal)
    check("dot: v·v = ‖v‖²",                          test_dot_self_is_norm_sq)
    check("dot: agrees with numpy",                   test_dot_agrees_with_numpy)
    check("norm_inf: max |v|",                        test_norm_inf_basic)
    check("norm_inf: zero vector → 0",                test_norm_inf_zero)
    check("norm_inf: uniform vector",                 test_norm_inf_positive)
    check("mat_vec: I*x = x",                         test_mat_vec_identity)
    check("mat_vec: [[1,2],[3,4]]*[1,1] = [3,7]",    test_mat_vec_known)
    check("mat_vec: 0*x = 0",                         test_mat_vec_zero_matrix)
    check("mat_vec: agrees with numpy @",             test_mat_vec_agrees_with_numpy)

    print(f"\n==================================")
    print(f"{_pass} passed, {_fail} failed / {_pass+_fail} total")
    sys.exit(0 if _fail == 0 else 1)
