#pragma once

#include "mc/dsp/fft/FFTBackend.hpp"
#include "mc/dsp/fft/FFTDirection.hpp"

#include "mc/dsp/fft/backend/KissFFT.hpp"
#include "mc/dsp/fft/backend/PFFFT.hpp"

#include "mc/algorithm.hpp"
#include "mc/complex.hpp"
#include "mc/memory.hpp"
#include "mc/preprocessor.hpp"

namespace mc::dsp
{

template<typename T, typename BackendTag>
struct FFT
{
    using value_type   = T;
    using backend_type = typename FFTBackend<T, BackendTag>::handle_type;

    FFT(int n, FFTDirection direction);
    ~FFT();

    [[nodiscard]] auto direction() const noexcept -> FFTDirection;
    [[nodiscard]] auto size() const noexcept -> int;

    auto perform(Complex<T> const* inp, Complex<T>* output) -> void;

private:
    int size_;
    FFTDirection direction_;
    backend_type engine_;
};

template<typename T, typename BackendTag>
FFT<T, BackendTag>::FFT(int n, FFTDirection direction)
    : size_{n}
    , direction_{direction}
    , engine_{FFTBackend<T, BackendTag>::construct(static_cast<std::size_t>(n), direction)}
{
}

template<typename T, typename BackendTag>
FFT<T, BackendTag>::~FFT()
{
    FFTBackend<T, BackendTag>::destroy(engine_);
}

template<typename T, typename BackendTag>
auto FFT<T, BackendTag>::direction() const noexcept -> FFTDirection
{
    return direction_;
}

template<typename T, typename BackendTag>
auto FFT<T, BackendTag>::size() const noexcept -> int
{
    return size_;
}

template<typename T, typename BackendTag>
auto FFT<T, BackendTag>::perform(Complex<T> const* input, Complex<T>* output) -> void
{
    FFTBackend<T, BackendTag>::perform(engine_, input, output, direction_);
}

struct RFFT
{
    RFFT(int n, FFTDirection direction);

    auto performRealToComplex(float const* inp, Complex<float>* oup) -> void;
    auto performComplexToReal(Complex<float> const* inp, float* oup) -> void;

private:
    std::unique_ptr<FFT<float, KissFFT>> cobj_;
    std::unique_ptr<Complex<float>[]> data_;
};

}  // namespace mc::dsp