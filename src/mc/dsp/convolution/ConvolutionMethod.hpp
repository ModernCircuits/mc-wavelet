#pragma once

#include "mc/preprocessor.hpp"
#include "mc/string.hpp"

namespace mc::dsp
{
enum struct ConvolutionMethod
{
    direct,
    fft,
};

[[nodiscard]] inline auto toString(ConvolutionMethod method) -> std::string
{
    if (method == ConvolutionMethod::direct) { return "direct"; }
    return "fft";
}
}  // namespace mc::dsp