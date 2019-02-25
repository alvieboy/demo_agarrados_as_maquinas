#ifndef __EFFECT_MOVE2_H__
#define __EFFECT_MOVE2_H__

#include "effect.h"

typedef struct {
} effect_move2_t;


void effect_move2_init(const effect_t *self,void*);
void effect_move2_activate(const effect_t *self,void*);
void effect_move2_tick(const effect_t *self,void*);

#endif
