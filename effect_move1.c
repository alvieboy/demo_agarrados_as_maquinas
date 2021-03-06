#include "effect_move1.h"
#include "strip.h"
#include "hsv.h"
#include "compiler.h"
#include "fixed.h"
#include "decay_accel.h"

typedef struct {
    uint8_t hsvcurve[128*3];
    uint8_t tick;
    uint8_t decaydata[] __attribute__((aligned));
} effect_move1_dyndata_t;


void effect_move1_init(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_move1_dyndata_t *dyn = (effect_move1_dyndata_t*)dyndata;

    compute_hsv(128, 127, dyn->hsvcurve);
    decay_accel__init( &dyn->decaydata[0], 16, FLOAT2FP16(0.0002) );   // 0.0002

    dyn->tick = 0;
}

void effect_move1_activate(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_move1_dyndata_t *dyn = (effect_move1_dyndata_t*)dyndata;
    dyn->tick = 127;
}

static uint32_t effect_indexed_color(effect_move1_dyndata_t *dyn,unsigned index)
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

static void effect_apply(effect_move1_dyndata_t *dyn)
{
    unsigned i;
    for (i=0;i<16;i++) {

        fixed_t v = decay_accel__get(&dyn->decaydata[0], i);
        v = fmul16(v, 127);
        if (v>127) {
            v=127;
        }

        uint32_t color = effect_indexed_color(dyn, v);
        // Bottom left
        strip__setpixel(15-i, color);
        strip__setpixel(16+i, color);
        strip__setpixel(32+15-i, color);
        strip__setpixel(32+16+i, color);
        strip__setpixel(64+15-i, color);
        strip__setpixel(64+16+i, color);
    }

}

void effect_move1_tick(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_move1_dyndata_t *dyn = (effect_move1_dyndata_t*)dyndata;
    int highlight = -1;

    // dxxxxyy
    if ((dyn->tick & 0x3)==0x3) {
        highlight = ((dyn->tick)>>2) & 15;
        if (dyn->tick&0x40) {
            highlight = 15 - highlight;
        }

        decay_accel__update( &dyn->decaydata[0], highlight, FLOAT2FP16(1.0) );
    }
    int i;
    for (i=0;i<16;i++) {
        if (i!=highlight) {
            decay_accel__update( &dyn->decaydata[0], i, FLOAT2FP16(0.0) );
        }
    }
    effect_apply(dyn);
    if (dyn->tick>0)
        dyn->tick--;
}
