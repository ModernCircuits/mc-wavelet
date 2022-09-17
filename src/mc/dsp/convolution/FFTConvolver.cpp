#include "FFTConvolver.hpp"

#include <mc/core/config.hpp>

#include <mc/core/algorithm.hpp>
#include <mc/core/cmath.hpp>
#include <mc/core/memory.hpp>

namespace mc::dsp {

namespace {
[[nodiscard]] auto factorf(std::size_t m) -> std::size_t
{
    auto n = m;
    while (n % 7 == 0) { n = n / 7; }
    while (n % 3 == 0) { n = n / 3; }
    while (n % 5 == 0) { n = n / 5; }
    while (n % 2 == 0) { n = n / 2; }
    return n;
}

[[nodiscard]] auto findnexte(std::size_t m) -> std::size_t
{
    auto n = m;
    while (factorf(n) != 1 || n % 2 != 0) { ++n; }
    return n;
}
}  // namespace

FFTConvolver::FFTConvolver(std::size_t signalSize, std::size_t patchSize)
    : _signalSize{signalSize}
    , _patchSize{patchSize}
    , _totalSize{findnexte(signalSize + _patchSize - 1U)}
    , _forwardFFT{makeUnique<RFFT>(_totalSize, FFTDirection::forward)}
    , _backwardFFT{makeUnique<RFFT>(_totalSize, FFTDirection::backward)}
{}

auto FFTConvolver::convolute(float const* signal, float const* patch, float* output) const
    -> void
{
    std::fill(_signalScratch.get(), std::next(_signalScratch.get(), _totalSize), 0.0F);
    std::fill(_patchScratch.get(), std::next(_patchScratch.get(), _totalSize), 0.0F);
    std::fill(_tmp.get(), std::next(_tmp.get(), _totalSize), 0.0F);
    std::fill(
        _signalScratchOut.get(),
        std::next(_signalScratchOut.get(), _totalSize),
        Complex<float>{}
    );
    std::fill(
        _patchScratchOut.get(),
        std::next(_patchScratchOut.get(), _totalSize),
        Complex<float>{}
    );
    std::fill(_tmpOut.get(), std::next(_tmpOut.get(), _totalSize), 0.0F);

    for (auto i = std::size_t{0}; i < _totalSize; i++) {
        if (i < _signalSize) { _signalScratch[i] = signal[i]; }
        if (i < _patchSize) { _patchScratch[i] = patch[i]; }
    }

    _forwardFFT->performRealToComplex(_signalScratch.get(), _signalScratchOut.get());
    _forwardFFT->performRealToComplex(_patchScratch.get(), _patchScratchOut.get());

    for (auto i = std::size_t{0}; i < _totalSize; i++) {
        _tmp[i] = _signalScratchOut[i] * _patchScratchOut[i];
    }

    _backwardFFT->performComplexToReal(_tmp.get(), _tmpOut.get());

    auto const ls = _signalSize + _patchSize - 1U;
    for (auto i = std::size_t{0}; i < ls; i++) { output[i] = _tmpOut[i] / _totalSize; }
}

}  // namespace mc::dsp
