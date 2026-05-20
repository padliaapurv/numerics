/*
 * gauss_jordan.h
 *
 * Safety-critical Gauss-Jordan elimination with partial pivoting.
 * Solves the n × n linear system  A * x = b.
 *
 * Safety constraints:
 *   - No dynamic memory allocation (working storage is on the stack)
 *   - No exceptions; all outcomes reported via Num_Status
 *   - No STL; plain C arrays only
 *   - Inputs A and b are const — never modified
 *   - Deterministic execution path for all valid inputs
 */

#ifndef GAUSS_JORDAN_H
#define GAUSS_JORDAN_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * gauss_jordan_solve
 *
 * Parameters:
 *   A  [in]  Coefficient matrix, row-major [NUM_MAX_N][NUM_MAX_N]. Read-only.
 *   b  [in]  Right-hand side vector [NUM_MAX_N]. Read-only.
 *   x  [out] Solution vector [NUM_MAX_N]. Written only on NUM_OK.
 *   n  [in]  System dimension. Must satisfy 1 <= n <= NUM_MAX_N.
 *
 * Returns:
 *   NUM_OK              Unique solution; x[] is valid.
 *   NUM_ERR_BAD_INPUT   n is out of [1, NUM_MAX_N].
 *   NUM_ERR_NO_SOLUTION System is inconsistent.
 *   NUM_ERR_INFINITE    System is underdetermined (free variables exist).
 *
 * Memory ownership:
 *   The augmented matrix is stack-allocated inside this function.
 *   A and b are never written. x is written only on NUM_OK.
 */
Num_Status gauss_jordan_solve(
    const double A[NUM_MAX_N][NUM_MAX_N],
    const double b[NUM_MAX_N],
    double       x[NUM_MAX_N],
    int          n
);

#ifdef __cplusplus
}
#endif

#endif /* GAUSS_JORDAN_H */
