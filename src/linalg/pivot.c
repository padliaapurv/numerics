#include "pivot.h"
#include "scalar.h"
#include <math.h>

int num_find_max_row(
    const double aug[NUM_MAX_N][NUM_MAX_N + 1],
    int          start_row,
    int          col,
    int          n
) {
    int    r, best = start_row;
    double best_val = fabs(aug[start_row][col]);

    for (r = start_row + 1; r < n; ++r) {
        double v = fabs(aug[r][col]);
        if (v > best_val) { best_val = v; best = r; }
    }

    return num_near_zero(best_val) ? -1 : best;
}
