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

#ifndef __DEFS_H__
#define __DEFS_H__

void Error_Handler(void);

#define DEBUG_USARTx USART1
#define LCD_RESET_POLARITY 0

#ifndef PARAM_UNUSED
#define PARAM_UNUSED __attribute__((unused))
#endif

#define LCD_DC_PORT     GPIOB
#define LCD_DC_PIN      GPIO_PIN_5

#define LCD_RST_PORT    GPIOB
#define LCD_RST_PIN     GPIO_PIN_4

#define SPI_CS0_PORT    GPIOB
#define SPI_CS0_PIN     GPIO_PIN_6

#define LCD_CS_PORT     SPI_CS0_PORT
#define LCD_CS_PIN      SPI_CS0_PIN


#define STRIP_ENABLE_PORT GPIOB
#define STRIP_ENABLE_PIN GPIO_PIN_9

#define ROTARY_A_PORT   GPIOA
#define ROTARY_A_PIN    GPIO_PIN_0

#define ROTARY_B_PORT   GPIOA
#define ROTARY_B_PIN    GPIO_PIN_1


#endif
