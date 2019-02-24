#ifndef __EFFECT_HSV_H__
#define __EFFECT_HSV_H__

#include "effect.h"

typedef struct {
    const uint32_t color;
    uint8_t tick;
} effect_hsv_t;


void effect_hsv_init(const effect_t *self,void*);
void effect_hsv_activate(const effect_t *self,void*);
void effect_hsv_tick(const effect_t *self,void*);

#endif
