#include <math.h>
#include "pav_analysis.h"

#define PI 3.14159265359

/*float compute_power(const float *x, unsigned int N) {
    float pot_num = 1e-12;
    float pot_den = 1e-12;
    float w;

    for(unsigned int n=0; n<N; n++){
        w = hamming_window_coef(n, N);
        pot_num += (x[n]*w)*(x[n]*w);
        pot_den += w*w;
    }
    return 10*log10(pot_num/pot_den);
}*/

float compute_power(const float *x, unsigned int N) {
    float pot = 1e-12;
    for(unsigned int n=0; n<N; n++){
        pot += x[n]*x[n];
    }
    return 10*log10(pot/N);
}

float compute_am(const float *x, unsigned int N) {
    float A = 0;
    for(unsigned int n=0; n<N ; n++){
        A += fabs(x[n]);
    }
    return A/N;
}

float compute_zcr(const float *x, unsigned int N, float fm) {
    float ZCR = 0;
    for(unsigned int n=1; n<N; n++){
        if(x[n]*x[n-1]<0){
            ZCR++;
        }
    }
    return fm*ZCR/(2*(N-1));
}

float hamming_window_coef(int n, int N){
    return 0.54 - 0.46 * cos((2*PI*n)/(N-1));
}
