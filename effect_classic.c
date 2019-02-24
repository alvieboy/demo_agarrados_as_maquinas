#include "effect_classic.h"
#include "strip.h"

typedef struct {
    uint8_t tick;
} effect_classic_dyndata_t;

void effect_classic_init(const effect_t *self __attribute__((unused)), void *dyndata)
{
//    effect_classic_t *pvt = (effect_classic_t*)self->pvt;
    effect_classic_dyndata_t *dyn = (effect_classic_dyndata_t*)dyndata;

    dyn->tick = 0;
}

void effect_classic_activate(const effect_t *self, void *dyndata)
{
    effect_classic_t *pvt = (effect_classic_t*)self->pvt;

    effect_classic_dyndata_t *dyn = (effect_classic_dyndata_t*)dyndata;
    dyn->tick = 127;
    strip__fill(pvt->color);
}


void effect_classic_tick(const effect_t *self __attribute__((unused)), void *dyndata)
{
    //effect_classic_t *pvt = (effect_classic_t*)self->pvt;
    effect_classic_dyndata_t *dyn = (effect_classic_dyndata_t*)dyndata;

    if (dyn->tick!=0) {
        dyn->tick--;
    } else {
        strip__fill(0x0);
    }
}
