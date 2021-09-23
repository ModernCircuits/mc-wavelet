/// \copyright Copyright (c) 2018, Rafat Hussain

#include "common.hpp"

#include "lt/dsp/fft/FFT.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

auto dwtPerStride(double const* inp, int n, double const* lpd, double const* hpd, int lpdLen, double* cA, int lenCA, double* cD, int istride, int ostride) -> void
{
    int l;
    int l2;
    int isodd;
    int i;
    int t;
    int lenAvg;
    int is;
    int os;

    lenAvg = lpdLen;
    l2 = lenAvg / 2;
    isodd = n % 2;

    for (i = 0; i < lenCA; ++i) {
        t = 2 * i + l2;
        os = i * ostride;
        cA[os] = 0.0;
        cD[os] = 0.0;
        for (l = 0; l < lenAvg; ++l) {
            if ((t - l) >= l2 && (t - l) < n) {
                is = (t - l) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) < l2 && (t - l) >= 0) {
                is = (t - l) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) < 0 && isodd == 0) {
                is = (t - l + n) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) < 0 && isodd == 1) {
                if ((t - l) != -1) {
                    is = (t - l + n + 1) * istride;
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[is];
                } else {
                    is = (n - 1) * istride;
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[is];
                }
            } else if ((t - l) >= n && isodd == 0) {
                is = (t - l - n) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) >= n && isodd == 1) {
                is = (t - l - (n + 1)) * istride;
                if (t - l != n) {
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[is];
                } else {
                    is = (n - 1) * istride;
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[is];
                }
            }
        }
    }
}

auto dwtSymStride(double const* inp, int n, double const* lpd, double const* hpd, int lpdLen, double* cA, int lenCA, double* cD, int istride, int ostride) -> void
{
    int i;
    int l;
    int t;
    int lenAvg;
    int is;
    int os;
    lenAvg = lpdLen;

    for (i = 0; i < lenCA; ++i) {
        t = 2 * i + 1;
        os = i * ostride;
        cA[os] = 0.0;
        cD[os] = 0.0;
        for (l = 0; l < lenAvg; ++l) {
            if ((t - l) >= 0 && (t - l) < n) {
                is = (t - l) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) < 0) {
                is = (-t + l - 1) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - l) >= n) {
                is = (2 * n - t + l - 1) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            }
        }
    }
}

auto modwtPerStride(int m, double const* inp, int /*N*/, double const* filt, int lpdLen, double* cA, int lenCA, double* cD, int istride, int ostride) -> void
{
    int l;
    int i;
    int t;
    int lenAvg;
    int is;
    int os;
    lenAvg = lpdLen;

    for (i = 0; i < lenCA; ++i) {
        t = i;
        os = i * ostride;
        is = t * istride;
        cA[os] = filt[0] * inp[is];
        cD[os] = filt[lenAvg] * inp[is];
        for (l = 1; l < lenAvg; l++) {
            t -= m;
            while (t >= lenCA) {
                t -= lenCA;
            }
            while (t < 0) {
                t += lenCA;
            }
            os = i * ostride;
            is = t * istride;
            cA[os] += filt[l] * inp[is];
            cD[os] += filt[lenAvg + l] * inp[is];
        }
    }
}

auto swtPerStride(int m, double const* inp, int n, double const* lpd, double const* hpd, int lpdLen, double* cA, int lenCA, double* cD, int istride, int ostride) -> void
{
    int l;
    int l2;
    int isodd;
    int i;
    int t;
    int lenAvg;
    int j;
    int is;
    int os;
    lenAvg = m * lpdLen;
    l2 = lenAvg / 2;
    isodd = n % 2;

    for (i = 0; i < lenCA; ++i) {
        t = i + l2;
        os = i * ostride;
        cA[os] = 0.0;
        cD[os] = 0.0;
        l = -1;
        for (j = 0; j < lenAvg; j += m) {
            l++;
            while (j >= lenCA) {
                j -= lenCA;
            }
            if ((t - j) >= l2 && (t - j) < n) {
                is = (t - j) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - j) < l2 && (t - j) >= 0) {
                is = (t - j) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - j) < 0) {
                is = (t - j + n) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - j) >= n && isodd == 0) {
                is = (t - j - n) * istride;
                cA[os] += lpd[l] * inp[is];
                cD[os] += hpd[l] * inp[is];
            } else if ((t - j) >= n && isodd == 1) {
                if (t - l != n) {
                    is = (t - j - (n + 1)) * istride;
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[is];
                } else {
                    is = (n - 1) * istride;
                    cA[os] += lpd[l] * inp[is];
                    cD[os] += hpd[l] * inp[n - 1];
                }
            }
        }
    }
}

auto idwtPerStride(double const* cA, int lenCA, double const* cD, double const* lpr, double const* hpr, int lprLen, double* x, int istride, int ostride) -> void
{
    int lenAvg;
    int i;
    int l;
    int m;
    int n;
    int t;
    int l2;
    int is;
    int ms;
    int ns;

    lenAvg = lprLen;
    l2 = lenAvg / 2;
    m = -2;
    n = -1;

    for (i = 0; i < lenCA + l2 - 1; ++i) {
        m += 2;
        n += 2;
        ms = m * ostride;
        ns = n * ostride;
        x[ms] = 0.0;
        x[ns] = 0.0;
        for (l = 0; l < l2; ++l) {
            t = 2 * l;
            if ((i - l) >= 0 && (i - l) < lenCA) {
                is = (i - l) * istride;
                x[ms] += lpr[t] * cA[is] + hpr[t] * cD[is];
                x[ns] += lpr[t + 1] * cA[is] + hpr[t + 1] * cD[is];
            } else if ((i - l) >= lenCA && (i - l) < lenCA + lenAvg - 1) {
                is = (i - l - lenCA) * istride;
                x[ms] += lpr[t] * cA[is] + hpr[t] * cD[is];
                x[ns] += lpr[t + 1] * cA[is] + hpr[t + 1] * cD[is];
            } else if ((i - l) < 0 && (i - l) > -l2) {
                is = (lenCA + i - l) * istride;
                x[ms] += lpr[t] * cA[is] + hpr[t] * cD[is];
                x[ns] += lpr[t + 1] * cA[is] + hpr[t + 1] * cD[is];
            }
        }
    }
}

auto idwtSymStride(double const* cA, int lenCA, double const* cD, double const* lpr, double const* hpr, int lprLen, double* x, int istride, int ostride) -> void
{
    int lenAvg;
    int i;
    int l;
    int m;
    int n;
    int t;
    int v;
    int ms;
    int ns;
    int is;
    lenAvg = lprLen;
    m = -2;
    n = -1;

    for (v = 0; v < lenCA; ++v) {
        i = v;
        m += 2;
        n += 2;
        ms = m * ostride;
        ns = n * ostride;
        x[ms] = 0.0;
        x[ns] = 0.0;
        for (l = 0; l < lenAvg / 2; ++l) {
            t = 2 * l;
            if ((i - l) >= 0 && (i - l) < lenCA) {
                is = (i - l) * istride;
                x[ms] += lpr[t] * cA[is] + hpr[t] * cD[is];
                x[ns] += lpr[t + 1] * cA[is] + hpr[t + 1] * cD[is];
            }
        }
    }
}

auto testSWTlength(int n, int j) -> int
{
    int ret;
    int div;
    int i;
    ret = 1;

    div = 1;
    for (i = 0; i < j; ++i) {
        div *= 2;
    }

    if ((n % div) != 0) {
        ret = 0;
    }

    return ret;
}

auto wmaxiter(int sigLen, int filtLen) -> int
{
    int lev;
    double temp;

    temp = std::log((double)sigLen / ((double)filtLen - 1.0)) / std::log(2.0);
    lev = (int)temp;

    return lev;
}