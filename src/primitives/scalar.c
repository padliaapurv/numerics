#include "scalar.h"
#include <math.h>

int num_near_zero(double x)
{
    return fabs(x) < NUM_EPSILON;
}
