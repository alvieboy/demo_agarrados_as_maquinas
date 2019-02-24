#ifndef __DEBUG_H__
#define __DEBUG_H__

#define debug__printf uart__printf
#define debug__putfp(x) do { \
    char tmp[16]; \
    fixed2ascii(x, FRACBITS, 4, 0, tmp);\
    uart__puts(tmp); } while (0);


#endif

