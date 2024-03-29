// SPDX-License-Identifier: BSL-1.0

#include <mc/fft/algorithm.hpp>

#include <mc/wavelet/algorithm.hpp>
#include <mc/wavelet/widget/denoise.hpp>

#include <mc/core/cmath.hpp>
#include <mc/core/cstdlib.hpp>
#include <mc/core/cstring.hpp>
#include <mc/core/memory.hpp>
#include <mc/core/print.hpp>
#include <mc/testing/test.hpp>

using namespace mc;

// modwtshrink can also be called from the denoise object.
// See denoisetest.cpp for more information.
auto main() -> int
{
    auto const* wname   = "db5";
    auto const* ext     = "per";
    auto const* thresh  = "soft";
    auto const* cmethod = "direct";

    auto const inp = readFileToVector<float>("test_data/raw/PieceRegular10.txt");
    auto sig       = readFileToVector<float>("test_data/raw/pieceregular1024.txt");

    auto const n = sig.size();
    auto const j = 4;
    auto out     = makeUnique<float[]>(n);

    modwtshrink(sig.data(), n, j, wname, cmethod, ext, thresh, out.get());

    mc::print("Signal - Noisy Signal Stats \n");
    mc::print("RMSE {}\n", rmsError(sig.data(), inp.data(), n));
    mc::print("Corr Coeff {}\n", corrcoef((int)n, sig.data(), inp.data()));

    mc::print("Signal - DeNoised Signal Stats \n");
    mc::print("RMSE {}\n", rmsError(sig.data(), out.get(), n));
    mc::print("Corr Coeff {}\n", corrcoef((int)n, sig.data(), out.get()));

    return 0;
}
