#include "wavelib.h"

#include "helper.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <memory>

auto main() -> int
{
    auto obj = Wavelet { "bior3.1" };

    auto const rows = 64;
    auto const cols = 48;
    auto const n = rows * cols;

    auto inp = makeZeros<double>(n);
    auto oup = makeZeros<double>(n);
    auto diff = makeZeros<double>(n);
    auto const j = 2;

    WaveletTransform2D* wt = wt2Init(obj, "swt", rows, cols, j);

    for (auto i = 0; i < rows; ++i) {
        for (auto k = 0; k < cols; ++k) {
            inp[i * cols + k] = generateRnd();
            oup[i * cols + k] = 0.0;
        }
    }

    auto wavecoeffs = swt2(wt, inp.get());

    int ir { 0 };
    int ic { 0 };
    auto* cLL = getWT2Coeffs(wt, wavecoeffs.get(), j, "A", &ir, &ic);

    dispWT2Coeffs(cLL, ir, ic);

    iswt2(wt, wavecoeffs.get(), oup.get());

    for (auto i = 0; i < n; ++i) {
        diff[i] = oup[i] - inp[i];
    }

    wt2Summary(wt);
    std::printf("Abs Max %g \n", absmax(diff.get(), n));

    wt2Free(wt);

    return 0;
}
