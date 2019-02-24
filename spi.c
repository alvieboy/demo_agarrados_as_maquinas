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

#include "spi.h"
#include "defs.h"

#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_spi.h>

static SPI_HandleTypeDef SpiHandle;

void spi__setprescaler(uint32_t prescale)
{
/*    SpiHandle.Init.BaudRatePrescaler = prescale;
    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        Error_Handler();
        }
        */
    uint32_t reg = READ_REG(SPI1->CR1);
    reg &= ~SPI_CR1_BR_Msk;
    reg |= prescale & SPI_CR1_BR_Msk;
    WRITE_REG(SPI1->CR1, reg);
}

void spi__init()
{
    /*##-1- Configure the SPI peripheral #######################################*/
    /* Set the SPI parameters */
    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_2EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial     = 7;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;

    SpiHandle.Init.Mode = SPI_MODE_MASTER;

    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }

    __HAL_SPI_ENABLE(&SpiHandle);
}



void spi__tx(uint8_t *buffer, uint32_t len)
{
    HAL_SPI_Transmit(&SpiHandle, buffer, len, 5000);
}

void spi__fasttx(uint8_t v)
{
#ifdef STM32F103xB
    while(__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE) == RESET);
    SpiHandle.Instance->DR = v;
#else
    spi__tx(&v,1);
#endif
}

void spi__fasttx16(uint16_t v)
{
#ifdef STM32F103xB
    while(__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE) == RESET);
    SpiHandle.Instance->DR = v;
#else
    uint8_t vv[2];
    vv[0] = v>>8;
    vv[1] = v;
    spi__tx(vv,2);
#endif
}

void spi__waittxe()
{
#ifdef STM32F103xB
    while(__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_TXE) == RESET);
#endif
}

void spi__waitnotbusy()
{
#ifdef STM32F103xB
    while(__HAL_SPI_GET_FLAG(&SpiHandle, SPI_FLAG_BSY));
#endif
}

void spi__txrx(uint8_t *buffer, uint8_t *ret, uint32_t len)
{
    HAL_SPI_TransmitReceive(&SpiHandle, buffer, ret, len, 5000);
}

void spi__setmode16()
{
#ifdef STM32F103xB
    SPI1->CR1 |= SPI_CR1_DFF;
#else
    SpiHandle.Init.DataSize          = SPI_DATASIZE_16BIT;

    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
#endif
}
void spi__setmode8()
{
#ifdef STM32F103xB
    SPI1->CR1 &= ~SPI_CR1_DFF;
#else
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
    if(HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        /* Initialization Error */
        Error_Handler();
    }
#endif
}

