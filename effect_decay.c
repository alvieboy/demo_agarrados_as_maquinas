#include "effect_decay.h"
#include "strip.h"
#include "decay_accel.h"

#define DECAY_MAX 127

typedef struct {
    uint32_t tick;
    uint8_t decaydata[];
} effect_decay_dyndata_t;

void effect_decay_init(const effect_t *self __attribute__((unused)), void *dyndata)
{
    effect_decay_dyndata_t *dyn = (effect_decay_dyndata_t*)dyndata;
    dyn->tick = 0;
}

void effect_decay_activate(const effect_t *self, void *dyndata)
{
    effect_decay_t *pvt = (effect_decay_t*)self->pvt;
    effect_decay_dyndata_t *dyn = (effect_decay_dyndata_t*)dyndata;
    dyn->tick = DECAY_MAX;
    strip__fill(pvt->color);
}

void effect_decay_tick(const effect_t *self, void *dyndata)
{
    effect_decay_t *pvt = (effect_decay_t*)self->pvt;
    effect_decay_dyndata_t *dyn = (effect_decay_dyndata_t*)dyndata;

    if (dyn->tick!=0) {
        uint32_t color;
        uint32_t final_color = 0;

        color = (pvt->color>>16) & 0xff;
        color *= dyn->tick;
        color /= DECAY_MAX;
        final_color<<=8;
        final_color|=(color &0xff);
        color = (pvt->color>>8) & 0xff;
        color *= dyn->tick;
        color /= DECAY_MAX;
        final_color<<=8;
        final_color|=(color &0xff);
        color = (pvt->color) & 0xff;
        color *= dyn->tick;
        color /= DECAY_MAX;
        final_color<<=8;
        final_color|=(color &0xff);
        strip__fill(final_color);
        dyn->tick--;

    } else {
        strip__fill(0x0);
    }
}
