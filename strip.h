#ifndef __STRIP_H__
#define __STRIP_H__

#include "defs.h"
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include "spi.h"

#define STRIP_LEN (16*3*2)
#define STRIP_FLUSH 32


static inline void strip__enable()
{
    HAL_GPIO_WritePin( STRIP_ENABLE_PORT, STRIP_ENABLE_PIN, 1);
    spi__setprescaler(SPI_BAUDRATEPRESCALER_4);
}

static inline void strip__disable()
{
    HAL_GPIO_WritePin( STRIP_ENABLE_PORT, STRIP_ENABLE_PIN, 0);
}

void strip__setpixel(unsigned, uint32_t);
void strip__fill(uint32_t color);
void strip__update(void);

#endif
