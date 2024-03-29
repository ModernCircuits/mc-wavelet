// SPDX-License-Identifier: BSL-1.0

#include <mc/wavelet/algorithm.hpp>

#include <mc/core/cmath.hpp>
#include <mc/core/cstdlib.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/memory.hpp>
#include <mc/core/print.hpp>
#include <mc/testing/test.hpp>
#include <mc/wavelet.hpp>

using namespace mc;

auto main() -> int
{
    auto obj     = Wavelet{"db4"};
    auto const n = size_t{788 + 23};
    auto const j = size_t{4};

    auto inp  = makeUnique<float[]>(n);
    auto oup  = makeUnique<float[]>(n);
    auto diff = makeUnique<float[]>(n);

    for (size_t i = 1; i < n + 1; ++i) { inp[i - 1] = static_cast<float>(i); }

    auto wt = WaveletPacketTransform(&obj, n, j);
    setDWPTExtension(wt, "per");
    setDWPTEntropy(wt, "logenergy", 0);

    dwpt(wt, inp.get());

    idwpt(wt, oup.get());

    for (size_t i = 0; i < n; ++i) { diff[i] = (inp[i] - oup[i]) / inp[i]; }

    print("{0}\n", wt);

    // If Reconstruction succeeded then the output should be a small value.
    print("\n MAX {} \n", absmax(diff.get(), wt.signalLength()));

    return 0;
}
