#ifndef __EFFECT_FFT_H__
#define __EFFECT_FFT_H__

#include "effect.h"

typedef struct {
} effect_fft_t;


void effect_fft_init(const effect_t *self,void*);
void effect_fft_activate(const effect_t *self,void*);
void effect_fft_tick(const effect_t *self,void*);

#endif
