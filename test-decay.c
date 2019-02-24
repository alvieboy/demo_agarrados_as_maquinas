#include "decay_accel.h"
#include <stdio.h>

uint8_t buf[8192];

int main()
{
    decay_accel__init( buf, 8, FLOAT2FP16(0.0002) );

    decay_accel__update( buf, 0, FLOAT2FP16(1.0) );
    for (int i=0;i<128;i++) {
        fixed_t v = decay_accel__get(buf, 0);
        fixed_t v2 = fmul16(v, 127);

        if (v>127) {
            v=127;
        }

        uint32_t color = v2+(v2<<8)+(v2<<16);


        printf("%d: %f %d 0x%08x\n", i, (double)v/(double)(1<<16), v2, color);
        decay_accel__update( buf, 0, FLOAT2FP16(0.0) );
    }

}