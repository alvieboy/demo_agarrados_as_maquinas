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

#include "lcd_oled.h"
#include "defs.h"
#include "spi.h"
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_spi.h>
#include "uart.h"
#include "font.h"
#include <string.h>

static uint8_t framebuffer[(128*64)/8];
static uint8_t dirty = 0;

void lcd_oled__fillScreen();


void lcd_oled__setreset(unsigned val)
{
    HAL_GPIO_WritePin( LCD_RST_PORT, LCD_RST_PIN, val);
}

static void lcd_oled__setdcrs(int value)
{
    HAL_GPIO_WritePin( LCD_DC_PORT, LCD_DC_PIN, value);
}

static void lcd_oled__setcs(int value)
{
    HAL_GPIO_WritePin( LCD_CS_PORT, LCD_CS_PIN, value);
    if (!value) {
        spi__setprescaler(SPI_BAUDRATEPRESCALER_2);
    }
}

static void lcd_oled__sendCommand(uint8_t index)
{
    lcd_oled__setdcrs(0);
    lcd_oled__setcs(0);
    spi__tx(&index,1);
    lcd_oled__setcs(1);
}

void lcd_oled__update(void)
{
    uint8_t v;
    unsigned i;
    if (!dirty)
        return;
    dirty=0;

    lcd_oled__setdcrs(0);
    lcd_oled__setcs(0);
    v = 0x10;
    spi__tx(&v,1);
    v = 0x00;
    spi__tx(&v,1);

    lcd_oled__setdcrs(1);

    for (i=0;i<sizeof(framebuffer); i++) {
#ifdef TRANSLATE_XY
        // Compute data
        uint8_v =

            mask
#else
        spi__fasttx(framebuffer[i]);
#endif
    }

    lcd_oled__setdcrs(0);
    lcd_oled__setcs(1);
}
#if 0

static uint8_t lcd_oled__readRegister(uint8_t address, uint8_t param)
{
    uint8_t data, txdata;

    lcd_oled__sendCommand(0xd9);
    lcd_oled__sendData8(0x10+param);
    lcd_oled__setdcrs(0);
    lcd_oled__setcs(0);
    spi__tx(&address,1);
    lcd_oled__setdcrs(1);
    lcd_oled__setcs(0);
    txdata = 0xa5;
    spi__txrx(&txdata, &data, 1);
    lcd_oled__setcs(1);
    return data;
}
#endif

#if 0
static uint8_t spi__readID(void)
{
    int i;
    uint8_t data[3];
    uint8_t expectedId[3] = {0x00, 0x93, 0x41};
    int valid=1;
    outstring("Reading SPI id\r\n");
    for(i=0;i<3;i++)
    {
        data[i]=lcd_oled__readRegister(0xd3,i+1);
        if(data[i] != expectedId[i])
        {
            valid=0;
        }
    }
    if(!valid)
    {
        outstring("Read TFT ID failed, ID should be 0x009341, but read ID = 0x");
        for(i=0;i<3;i++)
        {
            printhexbyte(data[i]);
        }
        outstring("\r\n");
    }
    return valid;
}
#endif

static void lcd_oled__initLCD(void)
{
#if 1
    lcd_oled__setreset(LCD_RESET_POLARITY);
    HAL_Delay(200);
    lcd_oled__setreset(!LCD_RESET_POLARITY);
    HAL_Delay(150);
#endif

    lcd_oled__setcs(1);
    lcd_oled__setdcrs(1);

    //lcd_oled__setled(0);
    HAL_Delay(100);
    //lcd_oled__setled(1);

#if 0
    int i;
    for (i=0;i<30;i++) {
        if (spi__readID()==1)
            break;
        HAL_Delay(500);
    }
#endif
    lcd_oled__sendCommand(0xAE); // Display off

    lcd_oled__sendCommand(0xD5); // Display clock divide ratio
    lcd_oled__sendCommand(0x80);

    lcd_oled__sendCommand(0xA8); // Multiplex ratio
    lcd_oled__sendCommand(0x3F);

    lcd_oled__sendCommand(0xD3); // Display offset
    lcd_oled__sendCommand(0x00);

    lcd_oled__sendCommand(0x40); // Display start line

    lcd_oled__sendCommand(0x8D); // Charge pump
    lcd_oled__sendCommand(0x1A);

    lcd_oled__sendCommand(0x20); // Memory mode
    lcd_oled__sendCommand(0x00);

    //  0x020, 0x002,		/* 2012-05-27: page addressing mode */
    //  0x0a1,				/* segment remap a0/a1*/
   // 0x0c8,				/* c0: scan dir normal, c8: reverse */

    lcd_oled__sendCommand(0xA1); // Segment remap (A0/A1)

    lcd_oled__sendCommand(0xC8); // COM output direction

    lcd_oled__sendCommand(0xDA); // COM hardware configuration
    lcd_oled__sendCommand(0x12);

    lcd_oled__sendCommand(0x81); // Contrast
    lcd_oled__sendCommand(0xCF); // Contrast

    lcd_oled__sendCommand(0xD9); // Precharge
    lcd_oled__sendCommand(0xF1);

    lcd_oled__sendCommand(0xDB); // VCOMH deselect
    lcd_oled__sendCommand(0x40);

    lcd_oled__sendCommand(0xA4); // All pixels off
    lcd_oled__sendCommand(0xA6); // Display not inverted

    lcd_oled__sendCommand(0xAF);
    /* Display on                   */

    //lcd_oled__clear();
}

void lcd_oled__clear()
{
    memset(framebuffer,0,sizeof(framebuffer));
    dirty=1;
}

static inline void lcd_oled__setpixel(int x, int y, int val)
{
    uint8_t mask = 1<<(y & 0x7);
    unsigned offset = x + (y>>3)*(LCD_WIDTH);
    if (val) {
        framebuffer[offset]|=mask;
    } else {
        framebuffer[offset]&=~mask;
    }
    dirty=1;
}

void lcd_oled__putpixel(int x, int y, int val)
{
    lcd_oled__setpixel(x,y,val);
}

void lcd_oled__drawChar16(const font_t *font, int x, int y, uint8_t ch)
{
    uint16_t v = 0xffff;
    ch -= font->start;

    uint16_t mask = (1<<(font->w-1));
    const uint8_t *ptr = &font->bitmap[(unsigned)ch*2*font->h];

    for (int cy=0;cy<font->h;cy++) {
        uint16_t row = ((uint16_t)ptr[0]<<8) + ptr[1];
        ptr+=2;

        for (int cx=0;cx<font->w;cx++) {
            if (row&mask) v=1; else v=0;
            row<<=1;
            lcd_oled__setpixel(x+cx,y,v);
        }
        y++;
    }
}

void lcd_oled__drawChar8(const font_t *font, int x, int y, uint8_t ch)
{
    uint16_t v;
    ch -= font->start;

    const uint8_t *ptr = &font->bitmap[(unsigned)ch*font->h];

    for (int cy=0;cy<font->h;cy++) {
        uint8_t row = *ptr++;
        for (int cx=0;cx<font->w;cx++) {
            if (row&0x80) v=1; else v=0;
            row<<=1;
            lcd_oled__setpixel(x+cx,y,v);
        }
        y++;
    }
}

void lcd_oled__drawChar32(const font_t *font, int x, int y, uint8_t ch)
{

    uint16_t v = 0xffff;

    ch -= '-';
    const uint8_t *ptr = &font->bitmap[(unsigned)ch*4*font->h];

    for (int cy=0;cy<font->h;cy++) {
        uint32_t row =
            ((uint32_t)ptr[0]<<24) +
            ((uint32_t)ptr[1]<<16) +
            ((uint32_t)ptr[2]<<8) +
            ptr[3];
        ptr+=4;

        for (int cx=0;cx<font->h;cx++) {
            if (row&0x80000000) v=1; else v=0;
            row<<=1;
            lcd_oled__setpixel(x+cx,y,v);
        }
        //spi__wait();
        y++;
       // lcd_oled__setPage(y, y+8);
    }
}

void lcd_oled__drawChar24(const font_t *font, int x, int y, uint8_t ch)
{
    uint16_t v = 0xffff;

    ch -= '-';
    const uint8_t *ptr = &font->bitmap[(unsigned)ch*3*font->h];

    for (int cy=0;cy<font->h;cy++) {
        uint32_t row =
            ((uint32_t)ptr[0]<<16) +
            ((uint32_t)ptr[1]<<8) +
            ((uint32_t)ptr[2]<<0);
        ptr+=3;

        for (int cx=0;cx<font->w;cx++) {
            if (row&0x800000) v=1; else v=0;
            row<<=1;
            lcd_oled__setpixel(x+cx,y,v);
        }
        //spi__wait();
        y++;
       // lcd_oled__setPage(y, y+8);
    }
}

void lcd_oled__drawString(const font_t*font,int x, int y, const char *ch)
{
    int xpos=x;
    while (*ch) {
        lcd_oled__drawChar(font,xpos,y,(uint32_t)*ch);
        if (font->spacing) {
            xpos+=font->spacing(font, *ch);
        } else {
            xpos+=font->w;
        }
        ch++;
    }
}

void lcd_oled__drawFilledRect(int x, int y, int w, int h)
{
#if 0
    unsigned count = w*h;
    lcd_oled__sendCommand(0x2c);
    lcd_oled__setdcrs(1);
    lcd_oled__setcs(0);
    spi__setmode16();
    while (count--) {
        spi__fasttx16(color);
    }
    spi__waitnotbusy();
    lcd_oled__setcs(1);
    spi__setmode8();
#endif
}

void lcd_oled__drawRect(int x, int y, int w, int h)
{
#if 0
    lcd_oled__drawFilledRect(x,y,w,1,color);   // H line
    lcd_oled__drawFilledRect(x,y+h,w,1,color); // H line
    if (h>2) {
        lcd_oled__drawFilledRect(x,y,1,h,color);   // V line
        lcd_oled__drawFilledRect(x+w,y,1,h,color);   // V line
    }
#endif
}

void lcd_oled__init()
{
    lcd_oled__initLCD();
}
