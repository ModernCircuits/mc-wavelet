// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <mc/core/algorithm.hpp>
#include <mc/core/cassert.hpp>
#include <mc/core/cstddef.hpp>
#include <mc/core/iterator.hpp>
#include <mc/core/span.hpp>

namespace mc {

template<typename T>
[[maybe_unused]] auto median(Span<T const> data) -> T
{
    if (data.empty()) { return T{}; }

    MC_ASSERT(ranges::is_sorted(data));

    auto const size = data.size();
    auto const mid  = size / 2U;
    return size % 2U == 0U ? (data[mid] + data[mid - 1U]) / 2U : data[mid];
}

}  // namespace mc
