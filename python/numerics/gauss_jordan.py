"""
gauss_jordan.py — Pythonic wrapper around gauss_jordan_solve.
"""

import ctypes
import numpy as np

from ._bridge import (
    _lib, NumStatus, NUM_MAX_N,
    CDoubleVec, status_str, to_c_matrix, to_c_vec,
)


def solve(A, b) -> np.ndarray:
    """
    Solve the linear system  A @ x = b  using Gauss-Jordan elimination.

    Parameters
    ----------
    A : array-like, shape (n, n)
    b : array-like, shape (n,)

    Returns
    -------
    x : np.ndarray, shape (n,)

    Raises
    ------
    ValueError
        If the system has no solution, infinitely many solutions,
        dimensions are invalid, or n > NUM_MAX_N.
    """
    A = np.asarray(A, dtype=float)
    b = np.asarray(b, dtype=float)

    if b.ndim != 1:
        raise ValueError(f"b must be 1-D, got shape {b.shape}")
    n = len(b)
    if A.shape != (n, n):
        raise ValueError(f"A must be ({n}, {n}), got {A.shape}")
    if n < 1 or n > NUM_MAX_N:
        raise ValueError(f"n={n} is out of range [1, {NUM_MAX_N}]")

    A_c = to_c_matrix(A)
    b_c = to_c_vec(b)
    x_c = CDoubleVec()

    # ctypes arrays decay to pointers automatically when argtypes are declared.
    status = NumStatus(_lib.gauss_jordan_solve(A_c, b_c, x_c, ctypes.c_int(n)))

    if status != NumStatus.OK:
        raise ValueError(f"gauss_jordan_solve: {status_str(status)}")

    return np.array([x_c[i] for i in range(n)])
