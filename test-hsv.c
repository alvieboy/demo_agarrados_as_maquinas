#include "hsv.h"
#include <inttypes.h>
uint8_t tab[128*3];

int main()
{
    uint8_t *p= &tab[0];
    compute_hsv(128,127,tab);
    unsigned i;
    for (i=0;i<128;i++) {
        printf("%02x %02x %02x\n", *p++, *p++, *p++);
    }
}
