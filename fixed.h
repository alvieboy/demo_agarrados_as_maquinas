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

#ifndef __FIXED_H__
#define __FIXED_H__

#include <inttypes.h>

#define FLOAT2FP26(x) ((int32_t)((x)*(1<<26)))
#define FLOAT2FP16(x) ((int32_t)((x)*(1<<16)))
#define FIXED_6_26_ROUND_4_DECIMAL FLOAT2FP26(0.00005)
#define FIXED_6_26_ROUND_3_DECIMAL FLOAT2FP26(0.0005)
#define FIXED_6_26_ROUND_2_DECIMAL FLOAT2FP26(0.005)

typedef int32_t fixed_t;

void fixed2ascii_setspace(char c);
char* fixed2ascii(fixed_t fp, uint32_t fracbits, uint32_t decimal, int pad, char *dest);

static inline fixed_t fmul26(fixed_t a, fixed_t b)
{
    int64_t m = (int64_t)a * (int64_t)b;
    return m>>26;
}

static inline fixed_t fmul31(fixed_t a, fixed_t b)
{
    int64_t m = (int64_t)a * (int64_t)b;
    return m>>31;
}

static inline fixed_t fmul16(fixed_t a, fixed_t b)
{
    int64_t m = (int64_t)a * (int64_t)b;
    return m>>16;
}

fixed_t fexp16(fixed_t val);
fixed_t fln16(fixed_t val);

#endif
