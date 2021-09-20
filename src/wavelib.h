#ifndef WAVELIB_H_
#define WAVELIB_H_

#include <algorithm>
#include <memory>

#if defined(_MSC_VER)
#pragma warning(disable : 4200)
#pragma warning(disable : 4996)
#endif

#ifndef fft_type
#define fft_type double
#endif

#ifndef cplx_type
#define cplx_type double
#endif

template <typename T>
auto makeZeros(std::size_t length) -> std::unique_ptr<T[]>
{
    auto ptr = std::make_unique<T[]>(length);
    std::fill(ptr.get(), ptr.get() + length, T {});
    return ptr;
}

struct cplx_data {
    cplx_type re;
    cplx_type im;
};

struct wave_set {
    char wname[50];
    int filtlength; // When all filters are of the same length. [Matlab uses zero-padding to make all filters of the same length]
    int lpd_len; // Default filtlength = lpd_len = lpr_len = hpd_len = hpr_len
    int hpd_len;
    int lpr_len;
    int hpr_len;
    double* lpd;
    double* hpd;
    double* lpr;
    double* hpr;
    std::unique_ptr<double[]> params;
};

auto wave_init(char const* wname) -> wave_set*;

struct fft_data {
    fft_type re;
    fft_type im;
};

struct fft_set {
    int N;
    int sgn;
    int factors[64];
    int lf;
    int lt;
    std::unique_ptr<fft_data[]> data;
};

auto fft_init(int N, int sgn) -> fft_set*;

struct fft_real_set {
    fft_set* cobj;
    fft_data twiddle2[1];
};

auto fft_real_init(int N, int sgn) -> fft_real_set*;

struct conv_set {
    fft_real_set* fobj;
    fft_real_set* iobj;
    int ilen1;
    int ilen2;
    int clen;
};

auto conv_init(int N, int L) -> conv_set*;

struct wt_set {
    wave_set* wave;
    conv_set* cobj;
    char method[10];
    int siglength; // Length of the original signal.
    int modwtsiglength; // Modified signal length for MODWT
    int outlength; // Length of the output DWT vector
    int lenlength; // Length of the Output Dimension Vector "length"
    int J; // Number of decomposition Levels
    int MaxIter; // Maximum Iterations J <= MaxIter
    int even; // even = 1 if signal is of even length. even = 0 otherwise
    char ext[10]; // Type of Extension used - "per" or "sym"
    char cmethod[10]; // Convolution Method - "direct" or "FFT"

    int N; //
    int cfftset;
    int zpad;
    int length[102];
    double* output;
    std::unique_ptr<double[]> params;
};

auto wt_init(wave_set* wave, char const* method, int siglength, int J) -> wt_set*;

struct wtree_set {
    wave_set* wave;
    conv_set* cobj;
    char method[10];
    int siglength; // Length of the original signal.
    int outlength; // Length of the output DWT vector
    int lenlength; // Length of the Output Dimension Vector "length"
    int J; // Number of decomposition Levels
    int MaxIter; // Maximum Iterations J <= MaxIter
    int even; // even = 1 if signal is of even length. even = 0 otherwise
    char ext[10]; // Type of Extension used - "per" or "sym"

    int N; //
    int nodes;
    int cfftset;
    int zpad;
    int length[102];
    double* output;
    int* nodelength;
    int* coeflength;
    std::unique_ptr<double[]> params;
};

auto wtree_init(wave_set* wave, int siglength, int J) -> wtree_set*;

struct wpt_set {
    wave_set* wave;
    conv_set* cobj;
    int siglength; // Length of the original signal.
    int outlength; // Length of the output DWT vector
    int lenlength; // Length of the Output Dimension Vector "length"
    int J; // Number of decomposition Levels
    int MaxIter; // Maximum Iterations J <= MaxIter
    int even; // even = 1 if signal is of even length. even = 0 otherwise
    char ext[10]; // Type of Extension used - "per" or "sym"
    char entropy[20];
    double eparam;

    int N; //
    int nodes;
    int length[102];
    double* output;
    double* costvalues;
    double* basisvector;
    int* nodeindex;
    int* numnodeslevel;
    int* coeflength;
    std::unique_ptr<double[]> params;
};

auto wpt_init(wave_set* wave, int siglength, int J) -> wpt_set*;

struct cwt_set {
    char wave[10]; // Wavelet - morl/morlet,paul,dog/dgauss
    int siglength; // Length of Input Data
    int J; // Total Number of Scales
    double s0; // Smallest scale. It depends on the sampling rate. s0 <= 2 * dt for most wavelets
    double dt; // Sampling Rate
    double dj; // Separation between scales. eg., scale = s0 * 2 ^ ( [0:N-1] *dj ) or scale = s0 *[0:N-1] * dj
    char type[10]; // Scale Type - Power or Linear
    int pow; // Base of Power in case type = pow. Typical value is pow = 2
    int sflag;
    int pflag;
    int npad;
    int mother;
    double m; // Wavelet parameter param
    double smean; // Input Signal mean

    cplx_data* output;
    double* scale;
    double* period;
    double* coi;
    std::unique_ptr<double[]> params;
};

auto cwt_init(char const* wave, double param, int siglength, double dt, int J) -> cwt_set*;

struct wt2_set {
    wave_set* wave;
    char method[10];
    int rows; // Matrix Number of rows
    int cols; // Matrix Number of columns
    int outlength; // Length of the output DWT vector
    int J; // Number of decomposition Levels
    int MaxIter; // Maximum Iterations J <= MaxIter
    char ext[10]; // Type of Extension used - "per" or "sym"
    int coeffaccesslength;

    int N; //
    int* dimensions;
    int* coeffaccess;
    std::unique_ptr<int[]> params;
};

auto wt2_init(wave_set* wave, char const* method, int rows, int cols, int J) -> wt2_set*;

void dwt(wt_set* wt, double const* inp);

void idwt(wt_set* wt, double* dwtop);

auto getDWTmra(wt_set* wt, double* wavecoeffs) -> double*;

void wtree(wtree_set* wt, double const* inp);

void dwpt(wpt_set* wt, double const* inp);

void idwpt(wpt_set* wt, double* dwtop);

void swt(wt_set* wt, double const* inp);

void iswt(wt_set* wt, double* swtop);

auto getSWTmra(wt_set* wt, double* wavecoeffs) -> double*;

void modwt(wt_set* wt, double const* inp);

void imodwt(wt_set* wt, double* oup);

auto getMODWTmra(wt_set* wt, double* wavecoeffs) -> double*;

void setDWTExtension(wt_set* wt, char const* extension);

void setWTREEExtension(wtree_set* wt, char const* extension);

void setDWPTExtension(wpt_set* wt, char const* extension);

void setDWT2Extension(wt2_set* wt, char const* extension);

void setDWPTEntropy(wpt_set* wt, char const* entropy, double eparam);

void setWTConv(wt_set* wt, char const* cmethod);

auto getWTREENodelength(wtree_set* wt, int X) -> int;

void getWTREECoeffs(wtree_set* wt, int X, int Y, double* coeffs, int N);

auto getDWPTNodelength(wpt_set* wt, int X) -> int;

void getDWPTCoeffs(wpt_set* wt, int X, int Y, double* coeffs, int N);

void setCWTScales(cwt_set* wt, double s0, double dj, char const* type, int power);

void setCWTScaleVector(cwt_set* wt, double const* scale, int J, double s0, double dj);

void setCWTPadding(cwt_set* wt, int pad);

void cwt(cwt_set* wt, double const* inp);

void icwt(cwt_set* wt, double* cwtop);

auto getCWTScaleLength(int N) -> int;

auto dwt2(wt2_set* wt, double* inp) -> double*;

void idwt2(wt2_set* wt, double* wavecoeff, double* oup);

auto swt2(wt2_set* wt, double* inp) -> double*;

void iswt2(wt2_set* wt, double const* wavecoeffs, double* oup);

auto modwt2(wt2_set* wt, double* inp) -> double*;

void imodwt2(wt2_set* wt, double* wavecoeff, double* oup);

auto getWT2Coeffs(wt2_set* wt, double* wcoeffs, int level, char const* type, int* rows, int* cols) -> double*;

void dispWT2Coeffs(double* A, int row, int col);

void wave_summary(wave_set* obj);

void wt_summary(wt_set* wt);

void wtree_summary(wtree_set* wt);

void wpt_summary(wpt_set* wt);

void cwt_summary(cwt_set* wt);

void wt2_summary(wt2_set* wt);

void wave_free(wave_set* object);

void wt_free(wt_set* object);

void wtree_free(wtree_set* object);

void wpt_free(wpt_set* object);

void cwt_free(cwt_set* object);

void wt2_free(wt2_set* wt);

#endif /* WAVELIB_H_ */
