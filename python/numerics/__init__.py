"""
numerics — Python bindings for the safety-critical numerics core.

Quick start
-----------
>>> from numerics import gauss_jordan, matrix_ops
>>> x = gauss_jordan.solve([[2, 1], [1, 3]], [5, 10])
"""

from . import gauss_jordan
from . import matrix_ops

__all__ = ["gauss_jordan", "matrix_ops"]
