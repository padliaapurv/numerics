/*
 * numerics.h
 *
 * Shared configuration and status codes for the entire library.
 *
 * Safety constraints (apply to every .c that includes this header):
 *   - No dynamic memory allocation
 *   - No exceptions
 *   - No STL containers
 *   - Fixed-size stack buffers only
 *   - All errors returned via Num_Status; never via global state
 */

#ifndef NUMERICS_H
#define NUMERICS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Maximum matrix/vector dimension. Tune to available stack on target hardware. */
#define NUM_MAX_N   16

/* Values whose absolute magnitude is below this are treated as zero. */
#define NUM_EPSILON 1.0e-10

typedef enum {
    NUM_OK              =  0,   /* Operation completed successfully          */
    NUM_ERR_BAD_INPUT   = -1,   /* n out of range, or NULL-equivalent input */
    NUM_ERR_NO_SOLUTION = -2,   /* System is inconsistent                   */
    NUM_ERR_INFINITE    = -3,   /* System has infinitely many solutions     */
    NUM_ERR_SINGULAR    = -4    /* Matrix is singular or near-singular      */
} Num_Status;

/* Human-readable description of a status code. Never returns NULL. */
const char* num_status_str(Num_Status s);

#ifdef __cplusplus
}
#endif

#endif /* NUMERICS_H */
