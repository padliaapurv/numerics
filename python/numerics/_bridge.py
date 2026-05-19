"""
_bridge.py — ctypes bridge to libNumerics.so.

Loads the shared library, declares every C function's argtypes/restype,
and exports the raw ctypes types that the higher-level modules use to
stage arguments.  Nothing in this file is part of the public API.
"""

import ctypes
import pathlib
import enum

# ── Locate the shared library ─────────────────────────────────────────────────

_REPO_ROOT = pathlib.Path(__file__).resolve().parent.parent.parent
_LIB_PATH  = _REPO_ROOT / "build" / "libNumerics.so"

if not _LIB_PATH.exists():
    raise OSError(
        f"libNumerics.so not found at {_LIB_PATH}.\n"
        "Run  make lib  from the repo root first."
    )

_lib = ctypes.CDLL(str(_LIB_PATH))

# ── Constants mirroring numerics.h ────────────────────────────────────────────

NUM_MAX_N   = 16
NUM_EPSILON = 1.0e-10

# ── Status codes ──────────────────────────────────────────────────────────────

class NumStatus(enum.IntEnum):
    OK             =  0
    ERR_BAD_INPUT  = -1
    ERR_NO_SOLUTION = -2
    ERR_INFINITE   = -3
    ERR_SINGULAR   = -4

# ── Raw C array types ─────────────────────────────────────────────────────────

CDoubleRow    = ctypes.c_double * NUM_MAX_N          # double[NUM_MAX_N]
CDoubleMatrix = CDoubleRow * NUM_MAX_N               # double[NUM_MAX_N][NUM_MAX_N]
CDoubleVec    = ctypes.c_double * NUM_MAX_N          # double[NUM_MAX_N]
CDoublePtr    = ctypes.POINTER(ctypes.c_double)

# ── Function signatures ───────────────────────────────────────────────────────

_lib.gauss_jordan_solve.argtypes = [
    ctypes.POINTER(CDoubleRow),   # const double A[MAX_N][MAX_N]
    CDoublePtr,                    # const double b[MAX_N]
    CDoublePtr,                    # double       x[MAX_N]
    ctypes.c_int,                  # int n
]
_lib.gauss_jordan_solve.restype = ctypes.c_int

_lib.num_status_str.argtypes = [ctypes.c_int]
_lib.num_status_str.restype  = ctypes.c_char_p

_lib.num_swap_rows.argtypes  = [CDoublePtr, CDoublePtr, ctypes.c_int]
_lib.num_swap_rows.restype   = None

_lib.num_scale_row.argtypes  = [CDoublePtr, ctypes.c_double, ctypes.c_int]
_lib.num_scale_row.restype   = None

_lib.num_axpy_row.argtypes   = [CDoublePtr, CDoublePtr, ctypes.c_double, ctypes.c_int]
_lib.num_axpy_row.restype    = None

_lib.num_dot.argtypes        = [CDoublePtr, CDoublePtr, ctypes.c_int]
_lib.num_dot.restype         = ctypes.c_double

_lib.num_norm_inf.argtypes   = [CDoublePtr, ctypes.c_int]
_lib.num_norm_inf.restype    = ctypes.c_double

_lib.num_mat_vec.argtypes    = [
    ctypes.POINTER(CDoubleRow),   # const double A[MAX_N][MAX_N]
    CDoublePtr,                    # const double *x
    CDoublePtr,                    # double *y
    ctypes.c_int,
]
_lib.num_mat_vec.restype = None

# ── Helpers ───────────────────────────────────────────────────────────────────

def status_str(code: int) -> str:
    return _lib.num_status_str(code).decode()

def to_c_matrix(arr) -> CDoubleMatrix:
    """Convert a 2-D sequence to a CDoubleMatrix (zeroed beyond [n][n])."""
    import numpy as np
    arr = np.asarray(arr, dtype=float)
    n = arr.shape[0]
    m = CDoubleMatrix()
    for i in range(n):
        for j in range(n):
            m[i][j] = arr[i, j]
    return m

def to_c_vec(arr) -> CDoubleVec:
    """Convert a 1-D sequence to a CDoubleVec (zeroed beyond [n])."""
    import numpy as np
    arr = np.asarray(arr, dtype=float)
    v = CDoubleVec()
    for i, val in enumerate(arr):
        v[i] = val
    return v

def vec_ptr(arr):
    """Return a ctypes pointer to a contiguous float64 numpy array."""
    import numpy as np
    arr = np.ascontiguousarray(arr, dtype=np.float64)
    return arr.ctypes.data_as(CDoublePtr), arr   # keep arr alive
