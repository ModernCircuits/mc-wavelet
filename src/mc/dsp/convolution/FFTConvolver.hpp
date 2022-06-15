#pragma once

#include "mc/dsp/convolution/convolute.hpp"
#include "mc/dsp/fft/FFT.hpp"

namespace mc::dsp
{
struct FFTConvolver
{
    using value_type = float;

    FFTConvolver(std::size_t signalSize, std::size_t patchSize);

    auto convolute(float const* signal, float const* patch, float* output) const -> void;

private:
    std::size_t signalSize_;
    std::size_t patchSize_;
    std::size_t totalSize_;

    std::unique_ptr<RFFT> forwardFFT_;
    std::unique_ptr<RFFT> backwardFFT_;

    std::unique_ptr<float[]> signalScratch_{std::make_unique<float[]>(totalSize_)};
    std::unique_ptr<Complex<float>[]> signalScratchOut_ { std::make_unique<Complex<float>[]>(totalSize_) };

    std::unique_ptr<float[]> patchScratch_{std::make_unique<float[]>(totalSize_)};
    std::unique_ptr<Complex<float>[]> patchScratchOut_ { std::make_unique<Complex<float>[]>(totalSize_) };

    std::unique_ptr<Complex<float>[]> tmp_ { std::make_unique<Complex<float>[]>(totalSize_) };
    std::unique_ptr<float[]> tmpOut_{std::make_unique<float[]>(totalSize_)};
};
}  // namespace mc::dsp