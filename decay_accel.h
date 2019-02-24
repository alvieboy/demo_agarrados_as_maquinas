#ifndef __DECAY_ACCEL_H__
#define __DECAY_ACCEL_H__

#include "fixed.h"

void decay_accel__init(void *self, unsigned size, fixed_t acc);
void decay_accel__update(void *self, unsigned index, fixed_t level);
fixed_t decay_accel__get(void *self, unsigned index);

#endif
