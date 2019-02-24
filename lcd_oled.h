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

#ifndef __LCD_H__
#define __LCD_H__

#include <inttypes.h>
#include "font.h"

#ifdef __cplusplus
extern "C" {
#endif

#define LCD_WIDTH  128
#define LCD_HEIGHT 64

void lcd_oled__init(void);
void lcd_oled__clear(void);
void lcd_oled__setreset(unsigned);
void lcd_oled__update(void);
void lcd_oled__putpixel(int x, int y, int val);
void lcd_oled__drawString(const font_t*font, int x, int y, const char *ch);
void lcd_oled__drawFilledRect(int x, int y, int w, int h);
void lcd_oled__drawRect(int x, int y, int w, int h);
void lcd_oled__drawChar8(const font_t *font, int x, int y, uint8_t ch);
void lcd_oled__drawChar16(const font_t *font, int x, int y, uint8_t ch);
void lcd_oled__drawChar24(const font_t *font, int x, int y, uint8_t ch);
void lcd_oled__drawChar32(const font_t *font, int x, int y, uint8_t ch);

static inline void lcd_oled__drawChar(const font_t *font, int x, int y, uint8_t ch)
{
    if (font->w > 24) {
        lcd_oled__drawChar32(font, x, y, ch);
    } else if (font->w > 16) {
        lcd_oled__drawChar24(font, x, y, ch);
    } else if (font->w >8 ) {
        lcd_oled__drawChar16(font, x, y, ch);
    } else {
        lcd_oled__drawChar8(font, x, y, ch);
    }
}
#ifdef __cplusplus
}
#endif

#endif
