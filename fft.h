#ifndef __FFT_H__
#define __FFT_H__

#include "fixed.h"

void doFFT(fixed_t *in_real, fixed_t *in_im);
void doWindow(fixed_t *in_real);

#endif
