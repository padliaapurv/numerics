"""
test_gauss_jordan.py — Python-level tests for gauss_jordan.solve.

These run against the shared library via ctypes, exercising the full
call path: Python list → C array → solver → numpy result.
"""

import sys
import pathlib
import math

sys.path.insert(0, str(pathlib.Path(__file__).resolve().parent.parent))

from numerics import gauss_jordan

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

def near(a, b, tol=1e-6):
    return math.fabs(a - b) < tol

# ── Tests ─────────────────────────────────────────────────────────────────────

def test_1x1():
    x = gauss_jordan.solve([[5]], [15])
    assert near(x[0], 3.0), f"x={x[0]}"

def test_2x2():
    # 3x+2y=12, x-y=1  →  x=14/5, y=9/5
    x = gauss_jordan.solve([[3, 2], [1, -1]], [12, 1])
    assert near(x[0], 14/5) and near(x[1], 9/5), f"x={x}"

def test_3x3():
    A = [[2, 1, -1], [-3, -1, 2], [-2, 1, 2]]
    b = [8, -11, -3]
    x = gauss_jordan.solve(A, b)
    assert near(x[0], 2) and near(x[1], 3) and near(x[2], -1), f"x={x}"

def test_numpy_input():
    import numpy as np
    A = np.array([[4.0, -1], [-1, 3]])
    b = np.array([7.0, 8])
    x = gauss_jordan.solve(A, b)
    residual = np.linalg.norm(A @ x - b)
    assert residual < 1e-10, f"residual={residual}"

def test_solution_satisfies_ax_eq_b():
    import numpy as np
    A = [[2, -1, 0, 3], [1, 2, -1, 1], [-3, 0, 2, -2], [0, 1, 3, -1]]
    b = [7, 4, -5, 8]
    x = gauss_jordan.solve(A, b)
    residual = np.linalg.norm(np.array(A) @ x - np.array(b))
    assert residual < 1e-10, f"‖Ax-b‖={residual}"

def test_no_solution_raises():
    try:
        gauss_jordan.solve([[1, 1], [1, 1]], [1, 2])
        assert False, "should have raised ValueError"
    except ValueError as e:
        assert "no solution" in str(e).lower() or "inconsistent" in str(e).lower(), str(e)

def test_infinite_solutions_raises():
    try:
        gauss_jordan.solve([[1, 1], [2, 2]], [3, 6])
        assert False, "should have raised ValueError"
    except ValueError as e:
        assert "infinite" in str(e).lower() or "underdetermined" in str(e).lower(), str(e)

def test_bad_n_raises():
    try:
        gauss_jordan.solve([[1]], [1, 2])   # mismatched dims
        assert False, "should have raised"
    except ValueError:
        pass

def test_list_and_numpy_agree():
    import numpy as np
    A = [[3, 1], [1, 2]]
    b = [9, 8]
    x_list  = gauss_jordan.solve(A, b)
    x_numpy = gauss_jordan.solve(np.array(A, dtype=float), np.array(b, dtype=float))
    assert np.allclose(x_list, x_numpy), f"{x_list} vs {x_numpy}"

# ── Entry point ───────────────────────────────────────────────────────────────

if __name__ == "__main__":
    print("gauss_jordan (Python) — test suite")
    print("====================================\n")

    check("1x1  5x=15  →  x=3",                   test_1x1)
    check("2x2  unique solution",                   test_2x2)
    check("3x3  textbook  →  x=2,y=3,z=-1",        test_3x3)
    check("numpy array input accepted",             test_numpy_input)
    check("4x4  ‖Ax-b‖ < 1e-10",                   test_solution_satisfies_ax_eq_b)
    check("inconsistent  →  ValueError",            test_no_solution_raises)
    check("underdetermined  →  ValueError",         test_infinite_solutions_raises)
    check("dimension mismatch  →  ValueError",      test_bad_n_raises)
    check("list and numpy inputs agree",            test_list_and_numpy_agree)

    print(f"\n====================================")
    print(f"{_pass} passed, {_fail} failed / {_pass+_fail} total")
    sys.exit(0 if _fail == 0 else 1)
