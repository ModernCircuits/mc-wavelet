#include "lt/dsp/wavelets/Denoise.hpp"
#include "lt/cmath.hpp"

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <memory>

static auto rmse(int n, double const* x, double const* y) -> double
{
    double rms = NAN;
    int i = 0;

    rms = 0.0;

    for (i = 0; i < n; ++i) {
        rms += (x[i] - y[i]) * (x[i] - y[i]);
    }

    rms = std::sqrt(rms / (double)n);

    return rms;
}

static auto corrcoef(int n, double const* x, double const* y) -> double
{
    double cc = NAN;
    double xm = NAN;
    double ym = NAN;
    double tx = NAN;
    double ty = NAN;
    double num = NAN;
    double den1 = NAN;
    double den2 = NAN;
    int i = 0;
    xm = ym = 0.0;
    for (i = 0; i < n; ++i) {
        xm += x[i];
        ym += y[i];
    }

    xm = xm / n;
    ym = ym / n;
    num = den1 = den2 = 0.0;

    for (i = 0; i < n; ++i) {
        tx = x[i] - xm;
        ty = y[i] - ym;
        num += (tx * ty);
        den1 += (tx * tx);
        den2 += (ty * ty);
    }

    cc = num / std::sqrt(den1 * den2);

    return cc;
}

auto main() -> int
{
    char const* wname = "db5";
    char const* method = "dwt";
    char const* ext = "sym";
    char const* thresh = "soft";
    char const* level = "all";

    auto* ifp = std::fopen("testData/pieceregular1024.txt", "r");
    auto i = 0;
    if (ifp == nullptr) {
        std::printf("Cannot Open File");
        std::exit(EXIT_FAILURE);
    }

    double temp[2400];
    while (std::feof(ifp) == 0) {
        std::fscanf(ifp, "%lf \n", &temp[i]);
        i++;
    }
    std::fclose(ifp);

    auto n = i;
    auto j = 4;

    auto inp = std::make_unique<double[]>(n);
    auto oup = std::make_unique<double[]>(n);
    auto sig = std::make_unique<double[]>(n);

    for (i = 0; i < n; ++i) {
        sig[i] = temp[i];
    }

    ifp = std::fopen("testData/PieceRegular10.txt", "r");
    i = 0;
    if (ifp == nullptr) {
        std::printf("Cannot Open File");
        std::exit(EXIT_FAILURE);
    }

    while (std::feof(ifp) == 0) {
        std::fscanf(ifp, "%lf \n", &temp[i]);
        i++;
    }

    std::fclose(ifp);

    for (i = 0; i < n; ++i) {
        inp[i] = temp[i];
    }
    auto obj = DenoiseSet(n, j, wname);
    setDenoiseMethod(obj, "visushrink"); // sureshrink is also the default. The other option with dwt and swt is visushrink.
    // modwt works only with modwtshrink method
    setDenoiseWTMethod(obj, method); // Default is dwt. the other options are swt and modwt
    setDenoiseWTExtension(obj, ext); // Default is sym. the other option is per
    setDenoiseParameters(obj, thresh, level); // Default for thresh is soft. Other option is hard
    // Default for level is all. The other option is first

    denoise(obj, inp.get(), oup.get());

    // Alternative to denoise_set*
    // Just use visushrink, modwtshrink and sureshrink functions
    //visushrink(inp.get(),N,J,wname,method,ext,thresh,level,oup.get());
    //sureshrink(inp.get(),N,J,wname,method,ext,thresh,level,oup.get());
    // modwtshrink(sig.get(),N,J,wname,cmethod,ext,thresh,oup.get()); See modwtdenoisetest.c
    //ofp = std::fopen("testData/denoiseds.txt", "w");

    std::printf("Signal - Noisy Signal Stats \n");
    std::printf("RMSE %g\n", rmse(n, sig.get(), inp.get()));
    std::printf("Corr Coeff %g\n", corrcoef(n, sig.get(), inp.get()));

    std::printf("Signal - DeNoised Signal Stats \n");
    std::printf("RMSE %g\n", rmse(n, sig.get(), oup.get()));
    std::printf("Corr Coeff %g\n", corrcoef(n, sig.get(), oup.get()));

    return 0;
}
