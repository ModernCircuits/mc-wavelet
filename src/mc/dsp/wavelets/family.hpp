#pragma once

#include <mc/dsp/wavelets/filters/coif.hpp>
#include <mc/dsp/wavelets/filters/daubechies.hpp>
#include <mc/dsp/wavelets/filters/sym.hpp>
#include <mc/dsp/wavelets/wavelet_coefficients.hpp>

namespace mc::dsp {
template<typename T>
inline constexpr auto daubechiesWavelets = Array<WaveletCoefficients, 38>{
    WaveletCoefficients{ "db1",  daubechies1<T>,  2},
    WaveletCoefficients{ "db2",  daubechies2<T>,  4},
    WaveletCoefficients{ "db3",  daubechies3<T>,  6},
    WaveletCoefficients{ "db4",  daubechies4<T>,  8},
    WaveletCoefficients{ "db5",  daubechies5<T>, 10},
    WaveletCoefficients{ "db6",  daubechies6<T>, 12},
    WaveletCoefficients{ "db7",  daubechies7<T>, 14},
    WaveletCoefficients{ "db8",  daubechies8<T>, 16},
    WaveletCoefficients{ "db9",  daubechies9<T>, 18},
    WaveletCoefficients{"db10", daubechies10<T>, 20},
    WaveletCoefficients{"db11", daubechies11<T>, 22},
    WaveletCoefficients{"db12", daubechies12<T>, 24},
    WaveletCoefficients{"db13", daubechies13<T>, 26},
    WaveletCoefficients{"db14", daubechies14<T>, 28},
    WaveletCoefficients{"db15", daubechies15<T>, 30},
    WaveletCoefficients{"db16", daubechies16<T>, 32},
    WaveletCoefficients{"db17", daubechies17<T>, 34},
    WaveletCoefficients{"db18", daubechies18<T>, 36},
    WaveletCoefficients{"db19", daubechies19<T>, 38},
    WaveletCoefficients{"db20", daubechies20<T>, 40},
    WaveletCoefficients{"db21", daubechies21<T>, 42},
    WaveletCoefficients{"db22", daubechies22<T>, 44},
    WaveletCoefficients{"db23", daubechies23<T>, 46},
    WaveletCoefficients{"db24", daubechies24<T>, 48},
    WaveletCoefficients{"db25", daubechies25<T>, 50},
    WaveletCoefficients{"db26", daubechies26<T>, 52},
    WaveletCoefficients{"db27", daubechies27<T>, 54},
    WaveletCoefficients{"db28", daubechies28<T>, 56},
    WaveletCoefficients{"db29", daubechies29<T>, 58},
    WaveletCoefficients{"db30", daubechies30<T>, 60},
    WaveletCoefficients{"db31", daubechies31<T>, 62},
    WaveletCoefficients{"db32", daubechies32<T>, 64},
    WaveletCoefficients{"db33", daubechies33<T>, 66},
    WaveletCoefficients{"db34", daubechies34<T>, 68},
    WaveletCoefficients{"db35", daubechies35<T>, 70},
    WaveletCoefficients{"db36", daubechies36<T>, 72},
    WaveletCoefficients{"db37", daubechies37<T>, 74},
    WaveletCoefficients{"db38", daubechies38<T>, 76},
};

template<typename T>
inline constexpr auto symWavelets = Array<WaveletCoefficients, 19>{
    WaveletCoefficients{ "sym2",  sym2<T>,  4},
    WaveletCoefficients{ "sym3",  sym3<T>,  6},
    WaveletCoefficients{ "sym4",  sym4<T>,  8},
    WaveletCoefficients{ "sym5",  sym5<T>, 10},
    WaveletCoefficients{ "sym6",  sym6<T>, 12},
    WaveletCoefficients{ "sym7",  sym7<T>, 14},
    WaveletCoefficients{ "sym8",  sym8<T>, 16},
    WaveletCoefficients{ "sym9",  sym9<T>, 18},
    WaveletCoefficients{"sym10", sym10<T>, 20},
    WaveletCoefficients{"sym11", sym11<T>, 22},
    WaveletCoefficients{"sym12", sym12<T>, 24},
    WaveletCoefficients{"sym13", sym13<T>, 26},
    WaveletCoefficients{"sym14", sym14<T>, 28},
    WaveletCoefficients{"sym15", sym15<T>, 30},
    WaveletCoefficients{"sym16", sym16<T>, 32},
    WaveletCoefficients{"sym17", sym17<T>, 34},
    WaveletCoefficients{"sym18", sym18<T>, 36},
    WaveletCoefficients{"sym19", sym19<T>, 38},
    WaveletCoefficients{"sym20", sym20<T>, 40},
};

template<typename T>
inline constexpr auto coifWavelets = Array<WaveletCoefficients, 17>{
    WaveletCoefficients{ "coif1",  coif1<T>,   6},
    WaveletCoefficients{ "coif2",  coif2<T>,  12},
    WaveletCoefficients{ "coif3",  coif3<T>,  18},
    WaveletCoefficients{ "coif4",  coif4<T>,  24},
    WaveletCoefficients{ "coif5",  coif5<T>,  30},
    WaveletCoefficients{ "coif6",  coif6<T>,  36},
    WaveletCoefficients{ "coif7",  coif7<T>,  42},
    WaveletCoefficients{ "coif8",  coif8<T>,  48},
    WaveletCoefficients{ "coif9",  coif9<T>,  54},
    WaveletCoefficients{"coif10", coif10<T>,  60},
    WaveletCoefficients{"coif11", coif11<T>,  66},
    WaveletCoefficients{"coif12", coif12<T>,  72},
    WaveletCoefficients{"coif13", coif13<T>,  78},
    WaveletCoefficients{"coif14", coif14<T>,  84},
    WaveletCoefficients{"coif15", coif15<T>,  90},
    WaveletCoefficients{"coif16", coif16<T>,  96},
    WaveletCoefficients{"coif17", coif17<T>, 102},
};

template<typename T>
inline constexpr auto allWavelets = Array<WaveletCoefficients, 38 + 19 + 17>{
    WaveletCoefficients{   "db1",  daubechies1<T>,   2},
    WaveletCoefficients{   "db2",  daubechies2<T>,   4},
    WaveletCoefficients{   "db3",  daubechies3<T>,   6},
    WaveletCoefficients{   "db4",  daubechies4<T>,   8},
    WaveletCoefficients{   "db5",  daubechies5<T>,  10},
    WaveletCoefficients{   "db6",  daubechies6<T>,  12},
    WaveletCoefficients{   "db7",  daubechies7<T>,  14},
    WaveletCoefficients{   "db8",  daubechies8<T>,  16},
    WaveletCoefficients{   "db9",  daubechies9<T>,  18},
    WaveletCoefficients{  "db10", daubechies10<T>,  20},
    WaveletCoefficients{  "db11", daubechies11<T>,  22},
    WaveletCoefficients{  "db12", daubechies12<T>,  24},
    WaveletCoefficients{  "db13", daubechies13<T>,  26},
    WaveletCoefficients{  "db14", daubechies14<T>,  28},
    WaveletCoefficients{  "db15", daubechies15<T>,  30},
    WaveletCoefficients{  "db16", daubechies16<T>,  32},
    WaveletCoefficients{  "db17", daubechies17<T>,  34},
    WaveletCoefficients{  "db18", daubechies18<T>,  36},
    WaveletCoefficients{  "db19", daubechies19<T>,  38},
    WaveletCoefficients{  "db20", daubechies20<T>,  40},
    WaveletCoefficients{  "db21", daubechies21<T>,  42},
    WaveletCoefficients{  "db22", daubechies22<T>,  44},
    WaveletCoefficients{  "db23", daubechies23<T>,  46},
    WaveletCoefficients{  "db24", daubechies24<T>,  48},
    WaveletCoefficients{  "db25", daubechies25<T>,  50},
    WaveletCoefficients{  "db26", daubechies26<T>,  52},
    WaveletCoefficients{  "db27", daubechies27<T>,  54},
    WaveletCoefficients{  "db28", daubechies28<T>,  56},
    WaveletCoefficients{  "db29", daubechies29<T>,  58},
    WaveletCoefficients{  "db30", daubechies30<T>,  60},
    WaveletCoefficients{  "db31", daubechies31<T>,  62},
    WaveletCoefficients{  "db32", daubechies32<T>,  64},
    WaveletCoefficients{  "db33", daubechies33<T>,  66},
    WaveletCoefficients{  "db34", daubechies34<T>,  68},
    WaveletCoefficients{  "db35", daubechies35<T>,  70},
    WaveletCoefficients{  "db36", daubechies36<T>,  72},
    WaveletCoefficients{  "db37", daubechies37<T>,  74},
    WaveletCoefficients{  "db38", daubechies38<T>,  76},

    WaveletCoefficients{  "sym2",         sym2<T>,   4},
    WaveletCoefficients{  "sym3",         sym3<T>,   6},
    WaveletCoefficients{  "sym4",         sym4<T>,   8},
    WaveletCoefficients{  "sym5",         sym5<T>,  10},
    WaveletCoefficients{  "sym6",         sym6<T>,  12},
    WaveletCoefficients{  "sym7",         sym7<T>,  14},
    WaveletCoefficients{  "sym8",         sym8<T>,  16},
    WaveletCoefficients{  "sym9",         sym9<T>,  18},
    WaveletCoefficients{ "sym10",        sym10<T>,  20},
    WaveletCoefficients{ "sym11",        sym11<T>,  22},
    WaveletCoefficients{ "sym12",        sym12<T>,  24},
    WaveletCoefficients{ "sym13",        sym13<T>,  26},
    WaveletCoefficients{ "sym14",        sym14<T>,  28},
    WaveletCoefficients{ "sym15",        sym15<T>,  30},
    WaveletCoefficients{ "sym16",        sym16<T>,  32},
    WaveletCoefficients{ "sym17",        sym17<T>,  34},
    WaveletCoefficients{ "sym18",        sym18<T>,  36},
    WaveletCoefficients{ "sym19",        sym19<T>,  38},
    WaveletCoefficients{ "sym20",        sym20<T>,  40},

    WaveletCoefficients{ "coif1",        coif1<T>,   6},
    WaveletCoefficients{ "coif2",        coif2<T>,  12},
    WaveletCoefficients{ "coif3",        coif3<T>,  18},
    WaveletCoefficients{ "coif4",        coif4<T>,  24},
    WaveletCoefficients{ "coif5",        coif5<T>,  30},
    WaveletCoefficients{ "coif6",        coif6<T>,  36},
    WaveletCoefficients{ "coif7",        coif7<T>,  42},
    WaveletCoefficients{ "coif8",        coif8<T>,  48},
    WaveletCoefficients{ "coif9",        coif9<T>,  54},
    WaveletCoefficients{"coif10",       coif10<T>,  60},
    WaveletCoefficients{"coif11",       coif11<T>,  66},
    WaveletCoefficients{"coif12",       coif12<T>,  72},
    WaveletCoefficients{"coif13",       coif13<T>,  78},
    WaveletCoefficients{"coif14",       coif14<T>,  84},
    WaveletCoefficients{"coif15",       coif15<T>,  90},
    WaveletCoefficients{"coif16",       coif16<T>,  96},
    WaveletCoefficients{"coif17",       coif17<T>, 102},
};

}  // namespace mc::dsp