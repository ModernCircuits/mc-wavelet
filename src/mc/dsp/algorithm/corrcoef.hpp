#pragma once

#include <mc/core/cstddef.hpp>

namespace mc::dsp {

[[nodiscard]] auto corrcoef(int n, float const* x, float const* y) -> float;

}  // namespace mc::dsp