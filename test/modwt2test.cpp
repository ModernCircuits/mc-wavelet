// SPDX-License-Identifier: BSL-1.0

#include <mc/wavelet/algorithm.hpp>

#include <mc/core/cmath.hpp>
#include <mc/core/cstdlib.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/print.hpp>
#include <mc/testing/test.hpp>
#include <mc/wavelet.hpp>

using namespace mc;

auto main() -> int
{
    auto obj = Wavelet{"db2"};

    auto rows = 51;
    auto cols = 40;
    auto n    = rows * cols;

    auto inp  = makeZeros<float>(n);
    auto oup  = makeZeros<float>(n);
    auto diff = makeZeros<float>(n);

    auto j  = 2;
    auto wt = WaveletTransform2D(obj, "modwt", rows, cols, j);

    for (auto i = 0; i < rows; ++i) {
        for (auto k = 0; k < cols; ++k) {
            // inp[i*cols + k] = i*cols + k;
            inp[i * cols + k] = generateRnd();
            oup[i * cols + k] = 0.0F;
        }
    }

    auto wavecoeffs = modwt(wt, inp.get());

    int ir{0};
    int ic{0};
    getWT2Coeffs(wt, wavecoeffs.get(), j, "A", &ir, &ic);

    imodwt(wt, wavecoeffs.get(), oup.get());

    for (auto i = 0; i < n; ++i) { diff[i] = oup[i] - inp[i]; }

    print("{0}\n", wt);
    print("Abs Max {} \n", absmax(diff.get(), n));

    return 0;
}
