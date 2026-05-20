#include "numerics.h"

const char* num_status_str(Num_Status s)
{
    switch (s) {
        case NUM_OK:              return "OK";
        case NUM_ERR_BAD_INPUT:   return "ERROR: bad input";
        case NUM_ERR_NO_SOLUTION: return "ERROR: no solution (inconsistent system)";
        case NUM_ERR_INFINITE:    return "ERROR: infinite solutions (underdetermined)";
        case NUM_ERR_SINGULAR:    return "ERROR: singular or near-singular matrix";
        default:                  return "ERROR: unknown status";
    }
}
