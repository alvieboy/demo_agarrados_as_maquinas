#ifndef __EFFECT_DECAY_H__
#define __EFFECT_DECAY_H__

#include "effect.h"

typedef struct {
    const uint32_t color;
} effect_decay_t;

void effect_decay_init(const effect_t *self,void*);
void effect_decay_activate(const effect_t *self,void*);
void effect_decay_tick(const effect_t *self,void*);

#endif
