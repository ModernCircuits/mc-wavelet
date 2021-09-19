#include "wavelib.h"

#include "helper.hpp"

#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

auto main() -> int
{
    wave_object obj;
    wt2_object wt;
    int i;
    int k;
    int J;
    int rows;
    int cols;
    int N;
    double* inp;
    double* wavecoeffs;
    double* oup;
    double* diff;
    double* cLL;
    int ir;
    int ic;
    double amax;

    rows = 32;
    cols = 30;
    N = rows * cols;

    char const* name = "db2";
    obj = wave_init(name); // Initialize the wavelet
    srand(time(nullptr));
    inp = (double*)calloc(N, sizeof(double));
    oup = (double*)calloc(N, sizeof(double));
    diff = (double*)calloc(N, sizeof(double));

    J = 3;

    wt = wt2_init(obj, "dwt", rows, cols, J);

    for (i = 0; i < rows; ++i) {
        for (k = 0; k < cols; ++k) {
            //inp[i*cols + k] = i*cols + k;
            inp[i * cols + k] = generate_rnd();
            oup[i * cols + k] = 0.0;
        }
    }

    wavecoeffs = dwt2(wt, inp);

    cLL = getWT2Coeffs(wt, wavecoeffs, 1, "D", &ir, &ic);

    dispWT2Coeffs(cLL, ir, ic);

    idwt2(wt, wavecoeffs, oup);

    for (i = 0; i < rows * cols; ++i) {
        diff[i] = oup[i] - inp[i];
    }

    amax = absmax(diff, rows * cols);

    wt2_summary(wt);

    printf("Abs Max %g \n", amax);

    wave_free(obj);
    wt2_free(wt);
    free(inp);
    free(wavecoeffs);
    free(oup);
    free(diff);
    return 0;
}
