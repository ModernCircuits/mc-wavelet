#include "lt/dsp/wavelets.hpp"

#include "lt/cmath.hpp"
#include "lt/cstdlib.hpp"
#include "lt/cstring.hpp"
#include "lt/format.hpp"

#include "lt/testing/test.hpp"

auto main() -> int
{
    auto obj = Wavelet { "db2" };

    auto rows = 51;
    auto cols = 40;
    auto n = rows * cols;

    auto inp = makeZeros<double>(n);
    auto oup = makeZeros<double>(n);
    auto diff = makeZeros<double>(n);

    auto j = 2;
    auto wt = WaveletTransform2D(obj, "modwt", rows, cols, j);

    for (auto i = 0; i < rows; ++i) {
        for (auto k = 0; k < cols; ++k) {
            //inp[i*cols + k] = i*cols + k;
            inp[i * cols + k] = generateRnd();
            oup[i * cols + k] = 0.0;
        }
    }

    auto wavecoeffs = modwt(wt, inp.get());

    int ir { 0 };
    int ic { 0 };
    getWT2Coeffs(wt, wavecoeffs.get(), j, "A", &ir, &ic);

    imodwt(wt, wavecoeffs.get(), oup.get());

    for (auto i = 0; i < n; ++i) {
        diff[i] = oup[i] - inp[i];
    }

    summary(wt);
    fmt::printf("Abs Max %g \n", absmax(diff.get(), n));

    return 0;
}
