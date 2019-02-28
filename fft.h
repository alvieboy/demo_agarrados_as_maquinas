#ifndef __FFT_H__
#define __FFT_H__

#include "fixed.h"

void fft__doFFT(fixed_t *in_real, fixed_t *in_im);
void fft__applyWindow(fixed_t *in_real);

#endif
