// SPDX-License-Identifier: BSL-1.0

#pragma once

#include <mc/core/config.hpp>

#include <mc/fft/convolution.hpp>

#include <mc/wavelet/wavelet.hpp>

#include <mc/core/format.hpp>
#include <mc/core/span.hpp>
#include <mc/core/string.hpp>

namespace mc {
struct WaveletTree
{
    WaveletTree(Wavelet* waveIn, size_t signalLength, size_t j);

    auto extension(char const* newExtension) noexcept -> void;
    [[nodiscard]] auto extension() const noexcept -> String const&;

    auto nodeLength(size_t x) -> size_t;
    auto coeffs(size_t x, size_t y, float* coeffs, size_t n) const -> void;

    Wavelet* wave;
    String method;
    size_t siglength;  // Length of the original signal.
    size_t outlength;  // Length of the output DWT vector
    size_t lenlength;  // Length of the Output Dimension Vector "length"
    size_t J;          // Number of decomposition Levels
    size_t MaxIter;    // Maximum Iterations J <= MaxIter

    size_t N{};  //
    size_t nodes;
    size_t cfftset;
    size_t zpad{};
    size_t length[102]{};
    float* output;
    unsigned* coeflength;
    UniquePtr<float[]> params;
    unsigned* nodeLength_{nullptr};

private:
    String _ext;
};

auto wtree(WaveletTree& wt, float const* inp) -> void;

}  // namespace mc

template<>
struct fmt::formatter<mc::WaveletTree> : formatter<string_view>
{
    template<typename FormatContext>
    auto format(mc::WaveletTree const& wt, FormatContext& ctx) const
    {
        fmt::format_to(ctx.out(), "\n");
        fmt::format_to(ctx.out(), "{}\n", *wt.wave);
        fmt::format_to(ctx.out(), "Wavelet Transform : {} \n\n", wt.method.c_str());
        fmt::format_to(ctx.out(), "Signal Extension : {} \n\n", wt.extension().c_str());
        fmt::format_to(ctx.out(), "Number of Decomposition Levels {} \n\n", wt.J);
        fmt::format_to(ctx.out(), "Length of Input Signal {} \n\n", wt.siglength);
        fmt::format_to(ctx.out(), "Length of WT Output Vector {} \n\n", wt.outlength);
        fmt::format_to(ctx.out(), "coefficients are contained in vector : output \n\n");
        fmt::format_to(ctx.out(), "Coefficients Access \n");

        auto formatNode = [&](auto i, auto k, auto output, auto length) {
            fmt::format_to(
                ctx.out(),
                "Node {} {} Access : output[{}] Length : {} \n",
                i,
                k,
                output,
                length
            );
        };

        auto t  = 0;
        auto p2 = 2;
        for (size_t i = 0; i < wt.J; ++i) {
            for (auto k = 0; k < p2; ++k) {
                formatNode(i + 1, k, wt.nodeLength_[t], wt.length[wt.J - i]);
                t++;
            }
            p2 *= 2;
        }

        return fmt::format_to(ctx.out(), "\n");
    }
};
