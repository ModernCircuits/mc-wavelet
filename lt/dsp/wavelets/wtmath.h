/// \copyright Copyright (c) 2014, Rafat Hussain

#ifndef WTMATH_H_
#define WTMATH_H_

#include "filters/utility.hpp"

auto dwtPerStride(double const* inp, int n, double const* lpd, double const* hpd, int lpdLen,
    double* cA, int lenCA, double* cD, int istride, int ostride) -> void;

auto dwtSymStride(double const* inp, int n, double const* lpd, double const* hpd, int lpdLen,
    double* cA, int lenCA, double* cD, int istride, int ostride) -> void;

auto modwtPerStride(int m, double const* inp, int n, double const* filt, int lpdLen,
    double* cA, int lenCA, double* cD, int istride, int ostride) -> void;

auto swtPerStride(int m, double const* inp, int n, double const* lpd, double const* hpd, int lpdLen,
    double* cA, int lenCA, double* cD, int istride, int ostride) -> void;

auto idwtPerStride(double const* cA, int lenCA, double const* cD, double const* lpr, double const* hpr,
    int lprLen, double* x, int istride, int ostride) -> void;

auto idwtSymStride(double const* cA, int lenCA, double const* cD, double const* lpr, double const* hpr,
    int lprLen, double* x, int istride, int ostride) -> void;

auto imodwtPerStride(int m, double const* cA, int lenCA, double const* cD, double const* filt,
    int lf, double* x, int istride, int ostride) -> void;

auto idwt2Shift(int shift, int rows, int cols, double const* lpr, double const* hpr, int lf,
    double* a, double* h, double* v, double* d, double* oup) -> void;

auto downsamp(double const* x, int lenx, int m, double* y) -> int;

auto perExt(double const* sig, int len, int a, double* oup) -> int;

auto symmExt(double const* sig, int len, int a, double* oup) -> int;

auto testSWTlength(int n, int j) -> int;

auto wmaxiter(int sigLen, int filtLen) -> int;

#endif /* WAVELIB_H_ */
