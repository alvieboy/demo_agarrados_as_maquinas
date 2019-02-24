#ifndef __EFFECT_MOVE1_H__
#define __EFFECT_MOVE1_H__

#include "effect.h"

typedef struct {
    const uint32_t color;
    uint8_t tick;
} effect_move1_t;


void effect_move1_init(const effect_t *self,void*);
void effect_move1_activate(const effect_t *self,void*);
void effect_move1_tick(const effect_t *self,void*);

#endif
