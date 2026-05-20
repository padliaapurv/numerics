/*
 * scalar.h — scalar-level predicates.
 *
 * The lowest layer: no dependencies except numerics.h and <math.h>.
 * Every higher layer may include this.
 */

#ifndef SCALAR_H
#define SCALAR_H

#include "numerics.h"

#ifdef __cplusplus
extern "C" {
#endif

/* 1 if |x| < NUM_EPSILON, else 0. */
int num_near_zero(double x);

#ifdef __cplusplus
}
#endif

#endif /* SCALAR_H */
