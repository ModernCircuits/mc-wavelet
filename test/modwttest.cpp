#include "wavelib.h"
#include <cmath>
#include <cstdio>
#include <cstdlib>
#include <cstring>

auto absmax(double* array, int N) -> double
{
    double max;
    int i;

    max = 0.0;
    for (i = 0; i < N; ++i) {
        if (fabs(array[i]) >= max) {
            max = fabs(array[i]);
        }
    }

    return max;
}

auto main() -> int
{
    wave_object obj;
    wt_object wt;
    double* inp;
    double* out;
    double* diff;
    int N;
    int i;
    int J;

    FILE* ifp;
    double temp[1200];

    char const* name = "db4";
    obj = wave_init(name);
    wave_summary(obj);

    ifp = fopen("testData/signal.txt", "r");
    i = 0;
    if (ifp == nullptr) {
        printf("Cannot Open File");
        exit(100);
    }
    while (feof(ifp) == 0) {
        fscanf(ifp, "%lf \n", &temp[i]);
        i++;
    }
    N = 177;

    fclose(ifp);

    inp = (double*)malloc(sizeof(double) * N);
    out = (double*)malloc(sizeof(double) * N);
    diff = (double*)malloc(sizeof(double) * N);
    //wmean = mean(temp, N);

    for (i = 0; i < N; ++i) {
        inp[i] = temp[i];
        //printf("%g \n",inp[i]);
    }
    J = 2;

    wt = wt_init(obj, "modwt", N, J); // Initialize the wavelet transform object

    modwt(wt, inp); // Perform MODWT
    //MODWT output can be accessed using wt->output vector. Use wt_summary to find out how to extract appx and detail coefficients

    for (i = 0; i < wt->outlength; ++i) {
        printf("%g ", wt->output[i]);
    }

    imodwt(wt, out); // Perform ISWT (if needed)
    // Test Reconstruction

    for (i = 0; i < wt->siglength; ++i) {
        diff[i] = out[i] - inp[i];
    }

    printf("\n MAX %g \n", absmax(diff, wt->siglength)); // If Reconstruction succeeded then the output should be a small value.

    wt_summary(wt); // Prints the full summary.

    wave_free(obj);
    wt_free(wt);

    free(inp);
    free(out);
    free(diff);
    return 0;
}