#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal_rcc.h>
#include <stm32f1xx_hal_rcc.h>
#include "setup.h"
#include "uart.h"
#include "spi.h"
#include "debug.h"
#include "lcd_oled.h"
#include "defs.h"
#include "distance.h"
#include "font.h"
#include <stdbool.h>
#include "vsnprintf.h"
#include "pins.h"
#include "bit.h"
#include "cpu.h"
#include <string.h>
#include "hsv.h"
#include "effect.h"
#include "strip.h"
#include "adc.h"
#include "fft.h"

const uint8_t fft_indexes[32] = {
    8,9,10,11,
    12,14,15,16,
    18,20,21,24,
    27,30,32,36,
    38,40,43,48,
    54,60,64,72,
    81,86,96,108,
    121,128,144,161
};
fixed_t fft_to_display[32];

void tick__handler(void);

void set_led(uint8_t val);

void set_led(uint8_t val)
{
    HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, val);
}

static void gpio__setup()
{
    GPIO_InitTypeDef init;

    __HAL_RCC_GPIOA_CLK_ENABLE();
    __HAL_RCC_GPIOB_CLK_ENABLE();
    __HAL_RCC_GPIOC_CLK_ENABLE();

    __HAL_RCC_AFIO_CLK_ENABLE();

    __HAL_AFIO_REMAP_SWJ_DISABLE();
    __HAL_AFIO_REMAP_I2C1_DISABLE();
    __HAL_AFIO_REMAP_USART1_DISABLE();
    __HAL_AFIO_REMAP_SPI1_DISABLE();

    __HAL_RCC_USART1_CLK_ENABLE();

    __HAL_RCC_SPI1_CLK_ENABLE();
    __HAL_RCC_I2C1_CLK_DISABLE();


    // LED

    HAL_GPIO_WritePin( GPIOC, GPIO_PIN_13, 0);
    init.Pin = GPIO_PIN_13;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init( GPIOC, &init );

    // UART

    init.Pin = GPIO_PIN_9 | GPIO_PIN_10;
    init.Mode = GPIO_MODE_AF_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init( GPIOA, &init );
#if 0
    init.Pin = GPIO_PIN_13 | GPIO_PIN_14;
    init.Mode = GPIO_MODE_ANALOG;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_LOW;

    HAL_GPIO_Init( GPIOC, &init );
#endif

    // SPI CS
    init.Pin = LCD_CS_PIN;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_WritePin( LCD_CS_PORT, LCD_CS_PIN, 1);
    HAL_GPIO_Init( LCD_CS_PORT, &init );

    // SPI others
    init.Pin = GPIO_PIN_5 | GPIO_PIN_7;
    init.Mode = GPIO_MODE_AF_PP;
    init.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOA, &init );

    // SPI others
    //init.Pin = GPIO_PIN_6;
    //init.Mode = GPIO_MODE_AF_INPUT;
   // init.Speed = GPIO_SPEED_FREQ_HIGH;

    //HAL_GPIO_Init( GPIOA, &init );

    init.Pin = LCD_DC_PIN;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( LCD_DC_PORT, &init );

    init.Pin = LCD_RST_PIN;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( LCD_RST_PORT, &init );

    init.Pin = STRIP_ENABLE_PIN;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( STRIP_ENABLE_PORT, &init );

    init.Pin = ROTARY_A_PIN;
    init.Mode = GPIO_MODE_INPUT;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init( ROTARY_A_PORT, &init );

    init.Pin = ROTARY_B_PIN;
    init.Mode = GPIO_MODE_INPUT;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    init.Pull = GPIO_PULLUP;
    HAL_GPIO_Init( ROTARY_B_PORT, &init );

    // ADC
    init.Pin = GPIO_PIN_3;
    init.Mode = GPIO_MODE_ANALOG;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init( GPIOA, &init );


}


#define DEBOUNCE_COUNTER 20
#define NUM_PINS 2

static uint8_t debounce_count[NUM_PINS] = { 0 };

#define PIN_ROTARY MAKEPIN(PORT_B, 8)
#define PIN_ACTION MAKEPIN(PORT_B, 7)

static pin_t pins[] = {
    PIN_ROTARY,
    PIN_ACTION
};

uint8_t input_values;
uint8_t last_sample;

static void read_inputs(void)
{
    unsigned int i;
    unsigned int v;
    uint8_t mask = 0x1;

    for (i=0; i<NUM_PINS;i++) {
        pin_t pin = pins[i];

        GPIO_TypeDef *port = pin__port(pin);

        v = !HAL_GPIO_ReadPin(port, pin__pin(pin));

        if (debounce_count[i]==0) {
            if (v) {
                input_values|=mask;
            } else {
                input_values&=~mask;
            }
        }

        if (v!=GET_BIT(last_sample,i)) {
            debounce_count[i] = DEBOUNCE_COUNTER;

            if (v) {
                last_sample|=mask;
            } else {
                last_sample&=~mask;
            }
        }

        if (debounce_count[i]>0) {
            debounce_count[i]--;
        }
        mask<<=1;
    }
}










uint32_t tick10ms_counter = 0;
volatile uint8_t tick10ms;

#define TICK10MS 4

void tick__handler(void)
{
    tick10ms_counter++;
    if (tick10ms_counter>TICK10MS) {
        tick10ms_counter=0;
        tick10ms=1;
    }
}



static void rotary__right(void);
static void rotary__left(void);
#if 0
static uint8_t rot_prev_a=1, rot_prev_b=1;

static void process_rotary(uint8_t a, uint8_t b)
{
    uint8_t rot_a = rot_prev_a;
    uint8_t rot_b = rot_prev_b;

    if (a != rot_a) {
        printf("A changed %d\n", a);
        rot_prev_a = a;
        if (b != rot_b) {
            printf("B changed %d\n", a);
            if (a==0) {
                if (rot_b) {
                    rotary__right();
                } else {
                    rotary__left();
                }
            }
        }
        rot_prev_b = b;
    }
}
#else
static int8_t rotary_dir=0;
static int8_t last_a=1, last_b=1;

static inline int rotary__falling_a(uint8_t val)
{
    return (val==0) && (last_a==1);
}

static inline int rotary__falling_b(uint8_t val)
{
    return (val==0) && (last_b==1);
}

static inline int rotary__rising_a(uint8_t val)
{
    return (val==1) && (last_a==0);
}

static inline int rotary__rising_b(uint8_t val)
{
    return (val==1) && (last_b==0);
}

static void process_rotary(uint8_t a, uint8_t b)
{
    if (rotary_dir==0) {
        if (rotary__falling_b(b) && a==0) {
            rotary_dir=1;
        } else if (rotary__falling_a(a) && b==0) {
            rotary_dir=-1;
        }
    } else if (rotary_dir==1) {
        if (rotary__rising_a(a) && b==0) {
            rotary_dir=0;
            rotary__right();
        }
    } else {
        if (rotary__rising_b(b) && a==0) {
            rotary_dir=0;
            rotary__left();
        }
    }
    last_a=a;
    last_b=b;
}

#endif

static void rotary__check()
{
    uint8_t a = HAL_GPIO_ReadPin(ROTARY_A_PORT, ROTARY_A_PIN);
    uint8_t b = HAL_GPIO_ReadPin(ROTARY_B_PORT, ROTARY_B_PIN);

    process_rotary(a,b);
}


static uint8_t latch_values = 0;
static void process_inputs()
{
    if (GET_BIT(input_values,0)) {
    }

    if ((GET_BIT(input_values,1)==1) && (GET_BIT(latch_values,1))==0) {
        effect__activate();
    }
    latch_values = input_values;
}

static void redraw()
{
    lcd_oled__clear();
    lcd_oled__drawString(font_get(FONT_8_16), 0,0," Tipo de Efeito ");

    unsigned l = strlen(effect()->title);

    lcd_oled__drawString(font_get(FONT_16_16), (LCD_WIDTH-(l*16))/2, 16, effect()->title);

    l = strlen(effect()->subtitle);
    lcd_oled__drawString(font_get(FONT_8_16), (LCD_WIDTH-(l*8))/2, 16+16+8, effect()->subtitle);
}

static void rotary__right(void)
{
    effect__next();
    redraw();
}
static void rotary__left(void)
{
    effect__previous();
    redraw();
}

uint16_t *adcdata = NULL;

void adc__conversion_complete_callback()
{
    adcdata = adc__raw();
}

fixed_t fft_real[512];
fixed_t fft_im[512];

static fixed_t fsqrt16( fixed_t x )
{
    uint32_t t, q, b, r;
    r = x;
    b = 0x40000000;
    q = 0;
    while( b > 0x40 )
    {
        t = q + b;
        if( r >= t )
        {
            r -= t;
            q = t + b; // equivalent to q += 2*b
        }
        r <<= 1;
        b >>= 1;
    }
    q >>= 8;
    return q;
}

static int get_magnitude(int index)
{
    fixed_t v = fft_real[index];
    //v.v>>=2;
    v = fmul16(v,v);
    fixed_t u = fft_im[index];
    //u.v>>=2;
    u = fmul16(u,u);
    v += u;
    // TODO: use hardware acceleration here, we already have a module
    v = fsqrt16(v);
    /*unsigned v1 = v.v;

    v1 = v1/256;
    if (v1>0xff)
    v1=0xff;
    return v1;
    */
    return v;//>>16;
}

fixed_t *getComputedFFT(void);

fixed_t *getComputedFFT(void)
{
    return &fft_to_display[0];
}


static void processADC()
{
    memset(fft_im,0,sizeof(fft_im));
    unsigned i;
    //int min = 0x7FFFFFFF;
    //char temp[64];
//    uart__printf("FFT input values: ");
    for (i=0;i<512;i++) {
        // Convert ADC values into 16.16 signed
        int32_t adc = adcdata[i];
  //      uart__printf("%d,", adc);
        adc -= 0x0000800; // 12-bit level
        adc <<= 4;
    //    uart__printf("%d\r\n", adc);
        fft_real[i] = adc;
    }
    //uart__puts("\r\n");
//    lcd_oled__drawString(font_get(FONT_8_16), 0,16+16+16,temp);
    doWindow(fft_real);
    doFFT(fft_real,fft_im);


    if (0) {
        uart__printf("FFT out real: ");
        for (i=0;i<256;i++) {
            uart__printf("%d\r\n", fft_real[i]);
        }
        uart__puts("\r\n");
        uart__printf("FFT out im: ");
        for (i=0;i<256;i++) {
            uart__printf("%d\r\n", fft_im[i]);
        }
        uart__puts("\r\n");
        uart__printf("FFT out mag: ");
        for (i=0;i<256;i++) {
            uart__printf("%d\r\n", get_magnitude(i));
        }
        uart__puts("\r\n");

    }

    //uart__printf("FFT set mag: ");
    for (i=0;i<32;i++) {
        fft_to_display[i] = get_magnitude(fft_indexes[i]);
      // uart__printf("%d ", fft_to_display[i]);
    }
    //uart__puts("\r\n");
    //adc__start();
    adcdata = NULL;

}
int tick_s=0;
#ifdef STM32F103xB
int main()
#else
int stm_main()
#endif
{
    clk__setup();
    SystemCoreClockUpdate();
    HAL_Init();
    gpio__setup();
    uart__init();
    outstring("Start\r\n");
    spi__init();
    lcd_oled__init();
    configure_inputs(pins, NUM_PINS);
    lcd_oled__clear();

    HAL_GPIO_WritePin( GPIOA, GPIO_PIN_13, 0);
    set_led(1);
    adc__init();
    adc__start();
    //compute_hsv(255,255,NULL);//unsigned steps, uint8_t max, uint8_t *dest);
    effect__init();
    redraw();
//    strip__setpixel(0, 0x0000FF);
    while (1) {
        rotary__check();
        read_inputs();
        process_inputs();
        cpu__wait();
        if (tick10ms) {
            tick_s++;
            if(tick_s==100) {
                tick_s=0;
                outstring("Tick\r\n");
            }
            tick10ms=0;
            lcd_oled__update();
            effect__tick();
            strip__update();
        }

        if (adcdata) {
            processADC();
        }
    }
}
