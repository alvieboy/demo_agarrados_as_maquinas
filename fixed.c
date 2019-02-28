/*
  ******************************************************************************
  * (C) 2018 Alvaro Lopes <alvieboy@alvie.com>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Alvaro Lopes nor the names of contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

#include "fixed.h"

static char space = ' ';

void fixed2ascii_setspace(char c)
{
    space = c;
}

char* fixed2ascii(fixed_t fp, uint32_t fracbits, uint32_t decimal, int pad, char *dest)
{
    uint8_t chars[32];
    int32_t i, n;
    int needsign = 0;

    // Convert integer part
    i=0;
    if (fp<0) {
        fp*=-1;
        needsign=1;
    }
    n = fp >> fracbits;
    do {
        chars[i++] = '0' + n % 10;
        n /= 10;
    } while (n!=0);
    if (needsign)
        chars[i++]='-';

    if (pad>0) {
        for (n=i;n<=pad;n++) {
            *dest++=space;
        }
    }

    i--;
    for (; i >= 0; i--)
        *dest++=chars[i];
    if (decimal>0) {
        *dest++='.';

        // Convert fractional part
        for (i = 0; i < (int)decimal; i++) {
            fp &= (1<<fracbits)-1;
            fp *= 10;
            *dest++=(fp >> fracbits) + '0';
        }
    }
    *dest='\0';
    return dest;
}

/* Computing the number of leading zeros in a word. */
static int clz(unsigned x)
{
    int n;

    /* See "Hacker's Delight" book for more details */
    if (x == 0) return 32;
    n = 0;
    if (x <= 0x0000FFFF) {n = n +16; x = x <<16;}
    if (x <= 0x00FFFFFF) {n = n + 8; x = x << 8;}
    if (x <= 0x0FFFFFFF) {n = n + 4; x = x << 4;}
    if (x <= 0x3FFFFFFF) {n = n + 2; x = x << 2;}
    if (x <= 0x7FFFFFFF) {n = n + 1;}

    return n;
}


fixed_t fln16(fixed_t val)
{
    int fracv, intv, y, ysq, fracr, bitpos;
    //const int ILN2 = 94548;        /* 1/ln(2) with 2^16 as base*/
    const int ILOG2E = 45426;    /* 1/log2(e) with 2^16 as base */

    const int ln_denoms[] = {
        (1<<16)/1,
        (1<<16)/3,
        (1<<16)/5,
        (1<<16)/7,
        (1<<16)/9,
        (1<<16)/11,
        (1<<16)/13,
        (1<<16)/15,
        (1<<16)/17,
        (1<<16)/19,
        (1<<16)/21,
    };

    /* compute fracv and intv */
    bitpos = 15 - clz(val);
    if(bitpos >= 0){
        ++bitpos;
        fracv = val>>bitpos;
    } else if(bitpos < 0){
        /* fracr = val / 2^-(bitpos) */
        ++bitpos;
        fracv = val<<(-bitpos);
    }

    // bitpos is the integer part of ln(val), but in log2, so we convert
    // ln(val) = log2(val) / log2(e)
    intv = bitpos * ILOG2E;

    // y = (ln_fraction_value−1)/(ln_fraction_value+1)
    y = ((int64_t)(fracv-(1<<16))<<16) / (fracv+(1<<16));

    ysq = (y*y)>>16;
    fracr = ln_denoms[10];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[9];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[8];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[7];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[6];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[5];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[4];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[3];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[2];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[1];
    fracr = (((int64_t)fracr * ysq)>>16) + ln_denoms[0];
    fracr =  ((int64_t)fracr * (y<<1))>>16;

    return intv + fracr;
}

fixed_t fexp16(fixed_t val)
{
    int x;

    x = val;
    x = x - (((int64_t)x*(fln16(x) - val))>>16);
    x = x - (((int64_t)x*(fln16(x) - val))>>16);
    x = x - (((int64_t)x*(fln16(x) - val))>>16);
    x = x - (((int64_t)x*(fln16(x) - val))>>16);
    return x;
}


