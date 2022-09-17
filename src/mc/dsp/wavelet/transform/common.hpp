#pragma once

#include <mc/core/cstddef.hpp>
#include <mc/core/memory.hpp>

namespace mc::dsp {

auto dwtPerStride(
    float const* inp,
    int n,
    float const* lpd,
    float const* hpd,
    int lpdLen,
    float* cA,
    int lenCA,
    float* cD,
    int istride,
    int ostride
) -> void;

auto dwtSymStride(
    float const* inp,
    int n,
    float const* lpd,
    float const* hpd,
    int lpdLen,
    float* cA,
    int lenCA,
    float* cD,
    int istride,
    int ostride
) -> void;

auto modwtPerStride(
    int m,
    float const* inp,
    int n,
    float const* filt,
    int lpdLen,
    float* cA,
    int lenCA,
    float* cD,
    int istride,
    int ostride
) -> void;

auto swtPerStride(
    int m,
    float const* inp,
    int n,
    float const* lpd,
    float const* hpd,
    int lpdLen,
    float* cA,
    int lenCA,
    float* cD,
    int istride,
    int ostride
) -> void;

auto idwtPerStride(
    float const* cA,
    int lenCA,
    float const* cD,
    float const* lpr,
    float const* hpr,
    int lprLen,
    float* x,
    int istride,
    int ostride
) -> void;

auto idwtSymStride(
    float const* cA,
    int lenCA,
    float const* cD,
    float const* lpr,
    float const* hpr,
    int lprLen,
    float* x,
    int istride,
    int ostride
) -> void;

auto testSWTlength(int n, int j) -> int;

auto maxIterations(std::size_t sigLen, std::size_t filtLen) -> std::size_t;

}  // namespace mc::dsp

namespace mc {
template<typename T>
auto makeZeros(std::size_t length) -> UniquePtr<T[]>
{
    auto ptr = makeUnique<T[]>(length);
    for (std::size_t i{0}; i < length; ++i) { ptr[i] = T{}; }
    return ptr;
}
}  // namespace mc