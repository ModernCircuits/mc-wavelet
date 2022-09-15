#include "mc/dsp/wavelets.hpp"

#include <mc/core/cmath.hpp>
#include <mc/core/cstdlib.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/format.hpp>
#include <mc/core/memory.hpp>
#include <mc/core/utility.hpp>

#include "mc/testing/test.hpp"

#include <fmt/printf.h>

using namespace mc;

auto main() -> int
{
    auto wave = dsp::Wavelet{"db4"};
    summary(wave);

    auto n = 177;
    auto j = 2;

    auto inp = readFileToVector<float>("test_data/raw/signal.txt");
    auto out = std::make_unique<float[]>(n);

    auto wt = dsp::WaveletTransform(wave, "modwt", n, j);

    // MODWT output can be accessed using wt.output vector.
    // Use wt_summary to find out how to extract appx and detail coefficients
    modwt(wt, inp.data());  // Perform MODWT

    for (auto i = 0; mc::cmp_less(i, wt.outlength); ++i) { fmt::printf("%g ", wt.output()[i]); }

    imodwt(wt, out.get());  // Perform ISWT (if needed)

    auto diff = std::make_unique<float[]>(n);
    for (auto i = 0; mc::cmp_less(i, wt.signalLength()); ++i) { diff[i] = out[i] - inp[i]; }

    // If Reconstruction succeeded then the output should be a small value.
    fmt::printf("\n MAX %g \n", absmax(diff.get(), wt.signalLength()));

    summary(wt);  // Prints the full summary.

    return 0;
}
