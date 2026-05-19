"""
matrix_ops.py — Pythonic wrappers around the shared row/vector primitives.

All functions accept plain Python lists or numpy arrays and return
numpy arrays.  In-place operations (swap, scale, axpy) return the
modified array(s) so they can be used in expressions.
"""

import ctypes
import numpy as np

from ._bridge import _lib, NUM_MAX_N, CDoublePtr, status_str


def _as_f64(arr) -> np.ndarray:
    return np.ascontiguousarray(arr, dtype=np.float64)


def _ptr(arr: np.ndarray) -> CDoublePtr:
    """Return a ctypes pointer into a contiguous float64 array's data."""
    return arr.ctypes.data_as(CDoublePtr)


# ── Row operations ────────────────────────────────────────────────────────────

def swap_rows(r1, r2):
    """
    Swap two equal-length vectors in place via the C primitive.

    Parameters
    ----------
    r1, r2 : array-like, same length

    Returns
    -------
    (r1, r2) as np.ndarray, with contents exchanged.
    """
    r1 = _as_f64(r1)
    r2 = _as_f64(r2)
    if len(r1) != len(r2):
        raise ValueError(f"r1 and r2 must have the same length ({len(r1)} vs {len(r2)})")
    _lib.num_swap_rows(_ptr(r1), _ptr(r2), ctypes.c_int(len(r1)))
    return r1, r2


def scale_row(row, scalar: float):
    """
    Scale row in place: row[i] *= scalar.

    Returns
    -------
    row as np.ndarray.
    """
    row = _as_f64(row)
    _lib.num_scale_row(_ptr(row), ctypes.c_double(scalar), ctypes.c_int(len(row)))
    return row


def axpy_row(dst, src, scalar: float):
    """
    AXPY in place: dst[i] += scalar * src[i].

    Returns
    -------
    dst as np.ndarray.
    """
    dst = _as_f64(dst)
    src = _as_f64(src)
    if len(dst) != len(src):
        raise ValueError(f"dst and src must have the same length ({len(dst)} vs {len(src)})")
    _lib.num_axpy_row(_ptr(dst), _ptr(src), ctypes.c_double(scalar), ctypes.c_int(len(dst)))
    return dst


# ── Vector operations ─────────────────────────────────────────────────────────

def dot(a, b) -> float:
    """Dot product: sum(a[i] * b[i])."""
    a = _as_f64(a)
    b = _as_f64(b)
    if len(a) != len(b):
        raise ValueError(f"a and b must have the same length ({len(a)} vs {len(b)})")
    return float(_lib.num_dot(_ptr(a), _ptr(b), ctypes.c_int(len(a))))


def norm_inf(v) -> float:
    """Infinity norm: max |v[i]|."""
    v = _as_f64(v)
    return float(_lib.num_norm_inf(_ptr(v), ctypes.c_int(len(v))))


# ── Matrix-vector product ─────────────────────────────────────────────────────

def mat_vec(A, x) -> np.ndarray:
    """
    Matrix-vector product  y = A @ x  via the C primitive.

    Parameters
    ----------
    A : array-like, shape (n, n),  n <= NUM_MAX_N
    x : array-like, shape (n,)

    Returns
    -------
    y : np.ndarray, shape (n,)
    """
    from ._bridge import CDoubleRow, CDoubleVec, to_c_matrix, to_c_vec

    A = np.asarray(A, dtype=float)
    x = np.asarray(x, dtype=float)
    n = len(x)
    if A.shape != (n, n):
        raise ValueError(f"A must be ({n}, {n}), got {A.shape}")
    if n > NUM_MAX_N:
        raise ValueError(f"n={n} exceeds NUM_MAX_N={NUM_MAX_N}")

    A_c = to_c_matrix(A)
    x_c = to_c_vec(x)
    y_c = CDoubleVec()

    _lib.num_mat_vec(A_c, x_c, y_c, ctypes.c_int(n))

    return np.array([y_c[i] for i in range(n)])
