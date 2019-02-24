/*
  ******************************************************************************
  * (C) 2018 Alvaro Lopes <alvieboy@alvie.com>
  * Parts (C) 2016 STMicroelectronics
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Alvaro Lopes, STMicroelectronics nor the names of contributors
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

#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_dma.h>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_spi.h>
#include "defs.h"
#include "uart.h"
#include "setup.h"

void clk__setup(void)
{
    unsigned int StartUpCounter=0;

    // enable HSE
    RCC->CR |= 0x00010001;

    // and wait for it to come on
    while ((RCC->CR & 0x00020000) == 0);

    // enable flash prefetch buffer
    FLASH->ACR = 0x00000012;
	
    // Configure PLL
#ifdef XTAL12M
    RCC->CFGR |= 0x00110400; /* pll=72Mhz(x6),APB1=36Mhz,AHB=72Mhz */
#else
    RCC->CFGR |= 0x001D0400; /* pll=72Mhz(x9),APB1=36Mhz,AHB=72Mhz */
    /*
     PPRE2 = 000
     PPRE1 = 100 (HCLK/2)
     */
#endif

    // enable the pll
    RCC->CR |= 0x01000000;

#ifndef HSE_STARTUP_TIMEOUT
  #define HSE_STARTUP_TIMEOUT    ((unsigned int)0x0500) /*!< Time out for HSE start up */
#endif /* HSE_STARTUP_TIMEOUT */   

    StartUpCounter = HSE_STARTUP_TIMEOUT;
    while (((RCC->CR & 0x03000000) == 0) && --StartUpCounter);
	
	if (!StartUpCounter) {
		// HSE has not started. Try restarting the processor
		Error_Handler();
	}
	
    // Set SYSCLK as PLL
    RCC->CFGR |= 0x00000002;
    // and wait for it to come on
    while ((RCC->CFGR & 0x00000008) == 0); 

    RCC->AHBENR = 0x0;
    RCC->APB2ENR = 0x1; // AFIO only
}

extern unsigned int _estack;

void __attribute__((noreturn)) Fault_Handler(unsigned *sp)
{
    unsigned int *spp = (unsigned int*)sp;
    outstring("Fault Handler\r\n\r\nSP: ");

    printhex((unsigned)sp);
    outstring(" ESP: ");
    printhex((unsigned)&_estack);
#define DREG(x) do { outstring("\r\n" #x ": 0x"); printhex(sp[0]); sp++; } while (0)

    DREG(r0);
    DREG(r1);
    DREG(r2);
    DREG(r3);
    DREG(r4);
    DREG(r5);
    DREG(r6);
    DREG(r7);
    DREG(r8);
    DREG(r9);
    DREG(r10);
    DREG(r11);
    DREG(r12);
    DREG(lr);

    outstring("\r\nSCnSCB->ACTLR : 0x");
    printhex(SCnSCB->ACTLR);

    outstring("\r\nSCB->SHCSR : 0x");
    printhex(SCB->SHCSR);

    outstring("\r\nSCB->CFSR : 0x");
    printhex(SCB->HFSR);

    outstring("\r\nSCB->HFSR : 0x");
    printhex(SCB->HFSR);
    outstring("\r\nSCB->DFSR : 0x");
    printhex(SCB->DFSR);
    outstring("\r\nSCB->MMFAR : 0x");
    printhex(SCB->MMFAR);
    outstring("\r\nSCB->BFAR : 0x");
    printhex(SCB->BFAR);
    outstring("\r\nSCB->AFSR : 0x");
    printhex(SCB->AFSR);

    outstring("\r\nStack trace:\r\n");
    while (spp<&_estack) {
        printhex(*spp);
        outstring("\r\n");
        spp++;
    }

    int c=0;
    while (1) {
        c++;
        HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, c&1);
        volatile int z=9000;
        while (z--) {}
    }
}
#if 0
void _init(void)
{
}
#endif