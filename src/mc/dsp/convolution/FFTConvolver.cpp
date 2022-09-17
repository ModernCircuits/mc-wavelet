#include "FFTConvolver.hpp"

#include <mc/dsp/algorithm/spectral_convolution.hpp>

#include <mc/core/algorithm.hpp>
#include <mc/core/bit.hpp>
#include <mc/core/cmath.hpp>
#include <mc/core/memory.hpp>

namespace mc::dsp {

FFTConvolver::FFTConvolver(std::size_t signalSize, std::size_t patchSize)
    : _signalSize{signalSize}
    , _patchSize{patchSize}
    , _totalSize{bit_ceil(signalSize + _patchSize - 1U)}
    , _forwardFFT{makeUnique<RFFT>(_totalSize, FFTDirection::forward)}
    , _backwardFFT{makeUnique<RFFT>(_totalSize, FFTDirection::backward)}
{
    _signalScratch.resize(_totalSize);
    _signalScratchOut.resize(_totalSize);
    _patchScratch.resize(_totalSize);
    _patchScratchOut.resize(_totalSize);
    _tmp.resize(_totalSize);
    _tmpOut.resize(_totalSize);
}

auto FFTConvolver::convolute(float const* signal, float const* patch, float* output) -> void
{
    ranges::fill(_signalScratch, 0.0F);
    ranges::fill(_patchScratch, 0.0F);
    ranges::fill(_tmp, 0.0F);

    ranges::fill(_signalScratchOut, Complex<float>{});
    ranges::fill(_patchScratchOut, Complex<float>{});
    ranges::fill(_tmpOut, 0.0F);

    for (auto i = std::size_t{0}; i < _totalSize; i++) {
        if (i < _signalSize) { _signalScratch[i] = signal[i]; }
        if (i < _patchSize) { _patchScratch[i] = patch[i]; }
    }

    _forwardFFT->performRealToComplex(_signalScratch.data(), _signalScratchOut.data());
    _forwardFFT->performRealToComplex(_patchScratch.data(), _patchScratchOut.data());

    spectralConvolution(_signalScratchOut, _patchScratchOut, _tmp);

    _backwardFFT->performComplexToReal(_tmp.data(), _tmpOut.data());

    auto const ls = _signalSize + _patchSize - 1U;
    for (auto i = std::size_t{0}; i < ls; i++) { output[i] = _tmpOut[i] / _totalSize; }
}

}  // namespace mc::dsp
