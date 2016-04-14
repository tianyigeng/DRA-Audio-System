#ifndef __FAST_MDCT_C_
#define __FAST_MDCT_C_

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "mdct_fft.h"

#define FFTW_MALLOC   fftw_malloc
#define FFTW_FREE     fftw_free
#define FFTW_PLAN_1D  fftw_plan_dft_1d
#define FFTW_DESTROY  fftw_destroy_plan 
#define FFTW_EXECUTE  fftw_execute

/* Window function */
static inline double _win_func(int M, int n) {
    return sin(M_PI * (n + 0.5) / (2.0 * M));
}

void mdct_free(struct mdct_plan* m_plan)
{
    if(m_plan) 
    {
        FFTW_DESTROY(m_plan->fft_plan);
        FFTW_FREE(m_plan->fft_in);
        FFTW_FREE(m_plan->fft_out);

        if(m_plan->twiddle)
            free(m_plan->twiddle);

        free(m_plan);
    }
}

#define MDCT_CLEAUP(msg, ...) \
    {fprintf(stderr, msg", %s(), %s:%d \n", \
            __VA_ARGS__, __func__, __FILE__, __LINE__); \
     mdct_free(m_plan); return NULL;}

struct mdct_plan* mdct_init(int M)
{
    int        n;
    FLOAT      alpha, omega, scale;
    struct mdct_plan* m_plan = NULL;

    if(0x00 != (M & 0x01)) 
        MDCT_CLEAUP(" Expect an even number of MDCT coeffs, but meet %d", M);

    m_plan = (struct mdct_plan*) malloc(sizeof(struct mdct_plan));
    if(NULL == m_plan)
        MDCT_CLEAUP(" malloc error: %s", "m_plan");
    memset(m_plan, 0, sizeof(m_plan[0]));

    m_plan->M = M;

    m_plan->twiddle = (FLOAT*) malloc(sizeof(FLOAT) * M);
    if(NULL == m_plan->twiddle)
        MDCT_CLEAUP(" malloc error: %s", "m_plan->twiddle");
    alpha = M_PI / (8.f * M);
    omega = M_PI / M;
    scale = sqrt(sqrt(2.f / M));    
    for(n = 0; n < (M >> 1); n++)
    {    
        m_plan->twiddle[2*n+0] = (FLOAT) (scale * cos(omega * n + alpha));
        m_plan->twiddle[2*n+1] = (FLOAT) (scale * sin(omega * n + alpha));
    }    
    
    m_plan->fft_in   
        = (FFTW_COMPLEX*) FFTW_MALLOC(sizeof(FFTW_COMPLEX) * M >> 1);    
    if(NULL == m_plan->fft_in)
        MDCT_CLEAUP(" malloc error: %s", "m_plan->fft_in");

    m_plan->fft_out  
        = (FFTW_COMPLEX*) FFTW_MALLOC(sizeof(FFTW_COMPLEX) * M >> 1);    
    if(NULL == m_plan->fft_out)
        MDCT_CLEAUP(" malloc error: %s", "m_plan->fft_out");

    m_plan->fft_plan = FFTW_PLAN_1D(M >> 1, 
                                    m_plan->fft_in, 
                                    m_plan->fft_out,    
                                    FFTW_FORWARD,
                                    FFTW_MEASURE);
    if(NULL == m_plan->fft_plan)
        MDCT_CLEAUP(" malloc error: %s", "m_plan->fft_plan");

    return m_plan;
}

void mdct(FLOAT* mdct_line, FLOAT* time_signal, struct mdct_plan* m_plan)
{
    FLOAT *xr, *xi, r0, i0;
    FLOAT *cos_tw, *sin_tw, c, s;
    int    M, M2, M32, M52, n;

    M   = m_plan->M;
    M2  = M >> 1;
    M32 = 3 * M2;
    M52 = 5 * M2;

    cos_tw = m_plan->twiddle;
    sin_tw = cos_tw + 1; 
    
    /* window applying */
    for (n = 0; n < 2 * M; n++) {
        time_signal[n] *= _win_func(M, n);
    }

    /* odd/even folding and pre-twiddle */
    xr = (FLOAT*) m_plan->fft_in;
    xi = xr + 1;
    for(n = 0; n < M2; n += 2) 
    {
        r0 = time_signal[M32-1-n] + time_signal[M32+n];    
        i0 = time_signal[M2+n]    - time_signal[M2-1-n];    
        
        c = cos_tw[n];
        s = sin_tw[n];

        xr[n] = r0 * c + i0 * s;
        xi[n] = i0 * c - r0 * s;
    }

    for(; n < M; n += 2) 
    {
        r0 = time_signal[M32-1-n] - time_signal[-M2+n];    
        i0 = time_signal[M2+n]    + time_signal[M52-1-n];    
        
        c = cos_tw[n];
        s = sin_tw[n];

        xr[n] = r0 * c + i0 * s;
        xi[n] = i0 * c - r0 * s;
    }

    /* complex FFT of size M/2 */
    FFTW_EXECUTE(m_plan->fft_plan);

    /* post-twiddle */
    xr = (FLOAT*) m_plan->fft_out;
    xi = xr + 1;
    for(n = 0; n < M; n += 2)
    {
        r0 = xr[n];
        i0 = xi[n];
        
        c = cos_tw[n];
        s = sin_tw[n];    

        mdct_line[n]     = - r0 * c - i0 * s;
        mdct_line[M-1-n] = - r0 * s + i0 * c;
    }
}

void imdct(FLOAT* time_signal, FLOAT* mdct_line, struct mdct_plan* m_plan)
{
    FLOAT *xr, *xi, r0, i0, r1, i1;
    FLOAT *cos_tw, *sin_tw, c, s;
    int    M, M2, M32, M52, n;

    M   = m_plan->M;
    M2  = M >> 1;
    M32 = 3 * M2;
    M52 = 5 * M2;

    cos_tw = m_plan->twiddle;
    sin_tw = cos_tw + 1; 
    
    /* pre-twiddle */
    xr = (FLOAT*) m_plan->fft_in;
    xi = xr + 1;
    for(n = 0; n < M; n += 2)
    {
        r0 =  mdct_line[n]; 
        i0 =  mdct_line[M-1-n];
        
        c = cos_tw[n];
        s = sin_tw[n];    
        
        xr[n] = -i0 * s - r0 * c;
        xi[n] = -i0 * c + r0 * s;
    } 
    
    /* complex FFT of size M/2 */
    FFTW_EXECUTE(m_plan->fft_plan);

    /* odd/even expanding and post-twiddle */
    xr = (FLOAT*) m_plan->fft_out;
    xi = xr + 1;
    for(n = 0; n < M2; n += 2) 
    {
        r0 = xr[n]; 
        i0 = xi[n];    
        
        c = cos_tw[n]; 
        s = sin_tw[n];

        r1 = r0 * c + i0 * s;
        i1 = r0 * s - i0 * c;

        time_signal[M32-1-n] =  r1;
        time_signal[M32+n]   =  r1;
        time_signal[M2+n]    =  i1;
        time_signal[M2-1-n]  = -i1;
    }

    for(; n < M; n += 2) 
    {
        r0 = xr[n]; 
        i0 = xi[n];    
        
        c = cos_tw[n]; 
        s = sin_tw[n];
        
        r1 = r0 * c + i0 * s;
        i1 = r0 * s - i0 * c;
        
        time_signal[M32-1-n] =  r1;
        time_signal[-M2+n]   = -r1;
        time_signal[M2+n]    =  i1;
        time_signal[M52-1-n] =  i1;
    }

    /* window applying */
    for (n = 0; n < 2 * M; n++) {
        time_signal[n] *= _win_func(M, n);
    }
}

#endif
