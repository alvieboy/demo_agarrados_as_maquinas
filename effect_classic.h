#ifndef __EFFECT_CLASSIC_H__
#define __EFFECT_CLASSIC_H__

#include "effect.h"

typedef struct {
    const uint32_t color;
    uint8_t tick;
} effect_classic_t;


void effect_classic_init(const effect_t *self, void *);
void effect_classic_activate(const effect_t *self, void*);
void effect_classic_tick(const effect_t *self,void*);

#endif
