#ifndef __EFFECT_H__
#define __EFFECT_H__

#include <inttypes.h>

typedef struct effect_t {
    const char *title;
    const char *subtitle;
    void (*init)(const struct effect_t *self, void *dyndata);
    void (*activate)(const struct effect_t *self, void *dyndata);
    void (*tick)(const struct effect_t *self, void *dyndata);
    const void *pvt;
} effect_t;


void effect__next(void);
void effect__previous(void);
const effect_t *effect(void);
void effect__tick(void);
void effect__activate(void);
void effect__init(void);

#endif
