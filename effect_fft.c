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
    uint8_t tick;
    uint8_t decaydata[] __attribute__((aligned));
} effect_fft_dyndata_t;

extern fixed_t *getComputedFFT();

void effect_fft_init(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    //    effect_fft_t *pvt = (effect_fft_t*)self->pvt;
    effect_fft_dyndata_t *dyn = (effect_fft_dyndata_t*)dyndata;


    compute_hsv(128, 127, dyn->hsvcurve);
    decay_accel__init( &dyn->decaydata[0], 32, FLOAT2FP16(0.00002) );   // 0.0002

    dyn->tick = 0;
}

void effect_fft_activate(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_fft_dyndata_t *dyn = (effect_fft_dyndata_t*)dyndata;
    //effect_fft_t *pvt = (effect_fft_t*)self->pvt;
    dyn->tick = 127;
}

static uint32_t effect_indexed_color(effect_fft_dyndata_t *dyn,unsigned index)
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
    //effect_fft_t *pvt = (effect_fft_t*)self->pvt;
    effect_fft_dyndata_t *dyn = (effect_fft_dyndata_t*)dyndata;
    int i;

    fixed_t *fft = getComputedFFT();
    subtick++;
    if (subtick>=10) {
//        outstring("G");
        adc__start();
        subtick = 0;
#if 0
        outstring("FFT:");
        for (i=0;i<32;i++) {
            uart__printf("%08x ", fft[i]);
        }
        outstring("\r\n");
#endif
    }
//    outstring("FFT:");
    for (i=0;i<32;i++) {
        //      uart__printf("%d ", fft[i]);
        fixed_t input = fft[i]>>6;

        //input = fexp16(input);
        //input = fmul16( input,  FLOAT2FP16(0.01 ));//1/2.718307) );

        if (input>FLOAT2FP16(1.0))
            input=FLOAT2FP16(1.0);
        decay_accel__update( &dyn->decaydata[0], i, input);
    }
    //adc__start();
//    outstring("\r\n");

    effect_apply(dyn);
}
