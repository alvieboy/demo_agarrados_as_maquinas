#include "strip.h"
#include "spi.h"

static unsigned char framebuffer[STRIP_LEN*3];
static volatile uint8_t strip_dirty=0;

void strip__setpixel(unsigned x, uint32_t grb)
{
    x*=3;
    framebuffer[x++] = grb>>16;
    framebuffer[x++] = grb>>8;
    framebuffer[x] = grb;
    strip_dirty=1;
}

void strip__fill(uint32_t color)
{
    unsigned i;
    uint8_t *ptr = framebuffer;
    for (i=0; i<STRIP_LEN; i++) {
        *ptr++ = color>>16;
        *ptr++ = color>>8;
        *ptr++ = color;
    }
    strip_dirty=1;
}

void strip__update()
{
    unsigned i;
    if (!strip_dirty)
        return;
    strip__enable();
    for (i=0;i<sizeof(framebuffer);i++) {
        spi__fasttx(framebuffer[i] | 0x80);
    }
    for (i=0;i<STRIP_FLUSH;i++) {
        spi__fasttx(0x0);
    }
    spi__waittxe();
    strip_dirty=0;
    strip__disable();
}
