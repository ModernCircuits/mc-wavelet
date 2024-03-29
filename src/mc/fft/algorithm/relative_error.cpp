#include "relative_error.hpp"

#include <mc/core/cmath.hpp>

namespace mc {

auto relError(float const* data, float const* rec, size_t n) -> float
{
    float sum1 = 0;
    float sum2 = 0;
    for (size_t i = 0; i < n; ++i) {
        sum1 += (data[i] - rec[i]) * (data[i] - rec[i]);
        sum2 += data[i] * data[i];
    }
    return sqrt(sum1) / sqrt(sum2);
}

}  // namespace mc
