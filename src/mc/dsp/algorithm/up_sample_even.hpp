#pragma once

#include <mc/core/algorithm.hpp>
#include <mc/core/cstddef.hpp>
#include <mc/core/utility.hpp>

namespace mc::dsp {

// Returns even numbered output. Last value is set to zero
template<typename SrcIt, typename DestIt>
auto upSampleEven(SrcIt srcF, SrcIt srcL, DestIt destF, std::size_t m) -> void
{
    using T = typename std::iterator_traits<SrcIt>::value_type;
    if (m == 0) { std::copy(srcF, srcL, destF); }

    auto it   = destF;
    auto last = destF + (m * static_cast<std::size_t>(std::distance(srcF, srcL)));
    auto j    = std::size_t{1};
    for (; it != last; ++it) {
        j--;
        *it = T(0);
        if (j == 0) {
            *it = *srcF;
            ++srcF;
            j = m;
        }
    }
}

}  // namespace mc::dsp