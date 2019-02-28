#include "decay_accel.h"

#define LOCAL_STRUCT(self) \
    uint8_t *__local_self=(uint8_t*)self;\
    unsigned *size = (unsigned*)__local_self; __local_self+=sizeof(unsigned); \
    fixed_t *accel = (fixed_t*)__local_self; __local_self+=sizeof(fixed_t); \
    fixed_t *lastlevel = (fixed_t*)__local_self; __local_self+=sizeof(fixed_t)*(*size); \
    uint16_t *decayindex = (uint16_t*)__local_self;


void decay_accel__init(void *self, unsigned sz, fixed_t acc)
{
    LOCAL_STRUCT(self);
    *accel = acc;
    *size = sz;
    unsigned i;
    for (i=0;i<sz;i++) {
        lastlevel[i]  = 0;
        decayindex[i] = 0;
    }
}

void decay_accel__update(void *self, unsigned index, fixed_t level)
{
    LOCAL_STRUCT(self);

    if (level>FLOAT2FP16(1.0))
        level=FLOAT2FP16(1.0);

    if (level >= lastlevel[index]) {
        lastlevel[index] = level;
        decayindex[index]=0;
    }
    else {
        // Decay.

        // X = Xo + vo(t) + 1/2at^2.

        if (lastlevel[index]>0) {
            fixed_t time = FLOAT2FP16(decayindex[index]);
            time = fmul16(time,time);
            time = fmul16(time, *accel);
            // Get previous in time
            fixed_t last;
            if (decayindex[index]>0) {
                last = FLOAT2FP16(decayindex[index]-1);
                last = fmul16(last,last);
                last = fmul16(last, *accel );
            } else {
                last = FLOAT2FP16(0);
            }
            time -= last;
            lastlevel[index] = lastlevel[index] - time;

            if (lastlevel[index]<0)
                lastlevel[index]=0;

            if (decayindex[index]<255)
                decayindex[index]++;
        }
    }
}

fixed_t decay_accel__get(void *self, unsigned index)
{
    LOCAL_STRUCT(self);
    (void)decayindex; // Unused
    (void)accel;      // Unused
    return lastlevel[index];
}
