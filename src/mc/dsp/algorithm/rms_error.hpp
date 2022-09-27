// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <mc/core/cstddef.hpp>

namespace mc {

[[nodiscard]] auto rmsError(float const* data, float const* rec, std::size_t n) -> float;

}  // namespace mc
