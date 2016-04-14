#ifndef __FAST_MDCT_H_
#define __FAST_MDCT_H_

/*
 *  This is a MDCT implementation based on FFT (Fast Fourier Transform)
 *  Time Complexity O(nlogn), faster than traditional approach O(n^2). 
 */

#include <fftw3.h>

typedef double        FLOAT;
typedef fftw_complex  FFTW_COMPLEX;
typedef fftw_plan     FFTW_PLAN;

struct mdct_plan {
    int           M;            // MDCT spectrum size (number of bins)
    FLOAT*        twiddle;      // twiddle factor
    FFTW_COMPLEX* fft_in;       // fft workspace, input
    FFTW_COMPLEX* fft_out;      // fft workspace, output
    FFTW_PLAN     fft_plan;     // fft configuration
}; 

struct mdct_plan* mdct_init(int M);    // MDCT spectrum size (number of bins)

void mdct_free(struct mdct_plan* m_plan);

void mdct(FLOAT* mdct_line, FLOAT* time_signal, struct mdct_plan* m_plan);

void imdct(FLOAT* time_signal, FLOAT* mdct_line, struct mdct_plan* m_plan);

#endif
