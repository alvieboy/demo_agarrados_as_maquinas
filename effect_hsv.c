#include "effect_hsv.h"
#include "strip.h"
#include "hsv.h"
#include "compiler.h"

typedef struct {
    uint8_t tick;
    uint8_t hsvcurve[128*3];
} effect_hsv_dyndata_t;


void effect_hsv_init(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    //    effect_hsv_t *pvt = (effect_hsv_t*)self->pvt;
    effect_hsv_dyndata_t *dyn = (effect_hsv_dyndata_t*)dyndata;


    compute_hsv(128, 127, dyn->hsvcurve);

    dyn->tick = 0;
}

void effect_hsv_activate(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    effect_hsv_dyndata_t *dyn = (effect_hsv_dyndata_t*)dyndata;
    //effect_hsv_t *pvt = (effect_hsv_t*)self->pvt;
    dyn->tick = 127;
}


void effect_hsv_tick(const effect_t *self PARAM_UNUSED, void *dyndata)
{
    //effect_hsv_t *pvt = (effect_hsv_t*)self->pvt;
    effect_hsv_dyndata_t *dyn = (effect_hsv_dyndata_t*)dyndata;

    if (dyn->tick!=0) {
        uint8_t *c = &dyn->hsvcurve[dyn->tick*3];
        uint32_t color = 0;

        color=*c++;
        color<<=8;
        color+=*c++;
        color<<=8;
        color+=*c;

        strip__fill(color);
        dyn->tick--;

    } else {
        strip__fill(0x0);
    }
}
