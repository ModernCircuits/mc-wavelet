#include "lt/cmath.hpp"
#include "lt/dsp/wavelets.hpp"

#include "lt/testing/test.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

auto main() -> int
{
    auto obj = Wavelet { "db4" };
    auto const n = 788 + 23;
    auto const j = 4;

    auto inp = std::make_unique<double[]>(n);
    auto oup = std::make_unique<double[]>(n);
    auto diff = std::make_unique<double[]>(n);

    for (auto i = 1; i < n + 1; ++i) {
        inp[i - 1] = i;
    }

    auto wt = WaveletPacketTransform(&obj, n, j);
    setDWPTExtension(wt, "per");
    setDWPTEntropy(wt, "logenergy", 0);

    dwt(wt, inp.get());

    idwt(wt, oup.get());

    for (auto i = 0; i < n; ++i) {
        diff[i] = (inp[i] - oup[i]) / inp[i];
    }

    summary(wt);

    // If Reconstruction succeeded then the output should be a small value.
    printf("\n MAX %g \n", absmax(diff.get(), wt.siglength));

    return 0;
}
