#include "effect_fft.h"
#include "strip.h"
#include "hsv.h"
#include "compiler.h"
#include "fixed.h"
#include "decay_accel.h"
#include "uart.h"
#include "adc.h"

typedef struct {
    uint8_t hsvcurve[128*3];
    uint8_t decaydata[] __attribute__((aligned));
} effect_fft_dyndata_t;

extern fixed_t *getComputedFFT();

void effect_fft_init(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_fft_dyndata_t *dyn = (effect_fft_dyndata_t*)dyndata;

    compute_hsv(128, 127, dyn->hsvcurve);

    decay_accel__init( &dyn->decaydata[0], 32, FLOAT2FP16(0.00002) );   // 0.0002
}

void effect_fft_activate(const effect_t *self PARAM_UNUSED, void *dyndata PARAM_UNUSED)
{
}

static uint32_t effect_indexed_color(effect_fft_dyndata_t *dyn, unsigned index)
{
    uint8_t *c = &dyn->hsvcurve[index*3];
    uint32_t color = 0;

    color=*c++;
    color<<=8;
    color+=*c++;
    color<<=8;
    color+=*c;
    return color;
}

static void effect_apply(effect_fft_dyndata_t *dyn)
{
    unsigned i;
    for (i=0;i<32;i++) {

        fixed_t v = decay_accel__get(&dyn->decaydata[0], i);
        v = fmul16(v, 127);
        if (v>127) {
            v=127;
        }

        uint32_t color = effect_indexed_color(dyn, v);
        // Bottom left
        strip__setpixel(i, color);
        strip__setpixel(63-i, color);
        strip__setpixel(64+i, color);
    }

}

volatile int subtick =0;

void effect_fft_tick(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_fft_dyndata_t *dyn = (effect_fft_dyndata_t*)dyndata;
    int i;
    fixed_t *fft = getComputedFFT();

    subtick++;

    if (subtick>=10) {
        adc__start(); // Restart ADC scan
        subtick = 0;
    }

    for (i=0;i<32;i++) {
        fixed_t input = fft[i]>>6;

        if (input>FLOAT2FP16(1.0))
            input=FLOAT2FP16(1.0);

        decay_accel__update( &dyn->decaydata[0], i, input);
    }

    effect_apply(dyn);
}
