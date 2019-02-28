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

#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_adc.h>
#include <stm32f1xx_hal_tim.h>
#include <stm32f1xx_hal_rcc.h>
#include "uart.h"
#include "defs.h"
#include "adc.h"
#include <string.h>

#undef USE_DMA
#undef TEST_SIGNAL

void ADC1_2_IRQHandler(void);
#ifdef USE_DMA
void DMA1_Channel1_IRQHandler(void);
static DMA_HandleTypeDef  AnalogDmaHandle;
#else
volatile unsigned adc_p = 0;
#endif

static ADC_HandleTypeDef  AdcHandle;


static __IO uint16_t   aADCxConvertedValues[ADCCONVERTEDVALUES_BUFFER_SIZE];

//static __IO fixed_t voltages[NUM_CHANNELS];



TIM_HandleTypeDef    TimHandle;
#define TIMER_FREQUENCY                ((uint32_t) 8372)    /* Timer frequency (unit: Hz). With a timer 16 bits and time base freq min 1Hz, range is min=1Hz, max=32kHz. */
#define TIMER_FREQUENCY_RANGE_MIN      ((uint32_t)    1)    /* Timer minimum frequency (unit: Hz). With a timer 16 bits, maximum frequency will be 32000 times this value. */
#define TIMER_PRESCALER_MAX_VALUE      (0xFFFF-1)           /* Timer prescaler maximum value (0xFFFF for a timer 16 bits) */

#define ADC_TIMx                            TIM3    /* Caution: Timer instance must be on APB1 (clocked by PCLK1) due to frequency computation in function "TIM_Config()" */
#define ADC_TIMx_CLK_ENABLE()               __HAL_RCC_TIM3_CLK_ENABLE()

#define ADC_TIMx_FORCE_RESET()              __HAL_RCC_TIM3_FORCE_RESET()
#define ADC_TIMx_RELEASE_RESET()            __HAL_RCC_TIM3_RELEASE_RESET()

#define ADC_EXTERNALTRIGCONV_Tx_TRGO    ADC_EXTERNALTRIGCONV_T3_TRGO

static void TIM_Config(void);

void adc__init(void)
{
  ADC_ChannelConfTypeDef   sConfig;
  
  /* Configuration of ADCx init structure: ADC parameters and regular group */
  AdcHandle.Instance = ADC1;
  
  AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;
  AdcHandle.Init.ScanConvMode          = ADC_SCAN_DISABLE;               /* Sequencer disabled (ADC conversion on only 1 channel: channel set on rank 1) */
  AdcHandle.Init.ContinuousConvMode    = DISABLE;                       /* Continuous mode to have maximum conversion speed (no delay between conversions) */
  AdcHandle.Init.NbrOfConversion       = 1;
  AdcHandle.Init.DiscontinuousConvMode = DISABLE;
  AdcHandle.Init.NbrOfDiscConversion   = 1;

  AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_Tx_TRGO;  /* Trig of conversion start done by external event */
#if 0
  AdcHandle.Init.ExternalTrigConv      = ADC_SOFTWARE_START;            /* Software start to trig the 1st conversion manually, without external event */
#endif

  if (HAL_ADC_Init(&AdcHandle) != HAL_OK)
  {
    /* ADC initialization error */
    Error_Handler();
  }
  
  /* Configuration of channel on ADCx regular group on sequencer rank 1 */
  /* Note: Considering IT occurring after each ADC conversion if ADC          */
  /*       conversion is out of the analog watchdog window selected (ADC IT   */
  /*       enabled), select sampling time and ADC clock with sufficient       */
  /*       duration to not create an overhead situation in IRQHandler.        */
  sConfig.Channel      = ADC_CHANNEL_3;
  sConfig.Rank         = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_41CYCLES_5;
  
  if (HAL_ADC_ConfigChannel(&AdcHandle, &sConfig) != HAL_OK)
  {
    /* Channel Configuration Error */
    Error_Handler();
  }

  TIM_Config();
  memset((void*)aADCxConvertedValues, 0xAA, sizeof(aADCxConvertedValues));
}

static void TIM_Config(void)
{
  TIM_MasterConfigTypeDef master_timer_config;
  RCC_ClkInitTypeDef clk_init_struct = {0};       /* Temporary variable to retrieve RCC clock configuration */
  uint32_t latency;                               /* Temporary variable to retrieve Flash Latency */
  
  uint32_t timer_clock_frequency = 0;             /* Timer clock frequency */
  uint32_t timer_prescaler = 0;                   /* Time base prescaler to have timebase aligned on minimum frequency possible */
  
  /* Configuration of timer as time base:                                     */ 
  /* Caution: Computation of frequency is done for a timer instance on APB1   */
  /*          (clocked by PCLK1)                                              */
  /* Timer period can be adjusted by modifying the following constants:       */
  /* - TIMER_FREQUENCY: timer frequency (unit: Hz).                           */
  /* - TIMER_FREQUENCY_RANGE_MIN: timer minimum frequency (unit: Hz).         */
  ADC_TIMx_CLK_ENABLE();

  /* Retrieve timer clock source frequency */
  HAL_RCC_GetClockConfig(&clk_init_struct, &latency);
  /* If APB1 prescaler is different of 1, timers have a factor x2 on their    */
  /* clock source.                                                            */
  if (clk_init_struct.APB1CLKDivider == RCC_HCLK_DIV1)
  {
    timer_clock_frequency = HAL_RCC_GetPCLK1Freq();
  }
  else
  {
    timer_clock_frequency = HAL_RCC_GetPCLK1Freq() *2;
  }
  
  /* Timer prescaler calculation */
  /* (computation for timer 16 bits, additional + 1 to round the prescaler up) */
  timer_prescaler = (timer_clock_frequency / (TIMER_PRESCALER_MAX_VALUE * TIMER_FREQUENCY_RANGE_MIN)) +1;
  
  /* Set timer instance */
  TimHandle.Instance = ADC_TIMx;
  
  /* Configure timer parameters */
  TimHandle.Init.Period            = ((timer_clock_frequency / (timer_prescaler * TIMER_FREQUENCY)) - 1);
  TimHandle.Init.Prescaler         = (timer_prescaler - 1);
  TimHandle.Init.ClockDivision     = TIM_CLOCKDIVISION_DIV1;
  TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
  TimHandle.Init.RepetitionCounter = 0x0;
  
  if (HAL_TIM_Base_Init(&TimHandle) != HAL_OK)
  {
    /* Timer initialization Error */
    Error_Handler();
  }

  /* Timer TRGO selection */
  master_timer_config.MasterOutputTrigger = TIM_TRGO_UPDATE;
  master_timer_config.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;

  if (HAL_TIMEx_MasterConfigSynchronization(&TimHandle, &master_timer_config) != HAL_OK)
  {
    /* Timer TRGO selection Error */
    Error_Handler();
  }
  HAL_TIM_Base_Start(&TimHandle);
}


void adc__calibrate()
{
    /* Run the ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&AdcHandle) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }
}

extern void set_led(uint8_t);
int adc_running = 0;

void adc__start()
{
#ifdef USE_DMA
    if (HAL_ADC_Start_DMA(&AdcHandle,
                          (uint32_t *)aADCxConvertedValues,
                          ADCCONVERTEDVALUES_BUFFER_SIZE
                         ) != HAL_OK)
    {
        /* Start Error */
        Error_Handler();
    }
#else
    if (!adc_running) {
        adc_running=1;
        adc_p =0 ;
        if (HAL_ADC_Start_IT(&AdcHandle) != HAL_OK)
        {
            /* Start Error */
            Error_Handler();
        }
    }
    outstring("S");
    if (adc_p>=ADCCONVERTEDVALUES_BUFFER_SIZE) {
        adc_p=0;
    }

#endif
}


#ifdef TEST_SIGNAL
extern uint16_t test_signal[512];
#endif

#ifndef USE_DMA

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if (adc_p<ADCCONVERTEDVALUES_BUFFER_SIZE) {
#ifdef TEST_SIGNAL
        aADCxConvertedValues[adc_p] = test_signal[adc_p];
#else
        aADCxConvertedValues[adc_p] = HAL_ADC_GetValue(hadc);
#endif
        adc_p++;
        if (adc_p==ADCCONVERTEDVALUES_BUFFER_SIZE) {
            //HAL_ADC_Stop_IT(hadc);
            outstring("F");

            adc__conversion_complete_callback();
        }
        set_led(0);
    } else {
        set_led(1);
    }

}
void ADC1_2_IRQHandler(void)
{
    HAL_ADC_IRQHandler(&AdcHandle);
}
#endif

#ifdef USE_DMA
void DMA1_Channel1_IRQHandler(void)
{
    HAL_DMA_IRQHandler(AdcHandle.DMA_Handle);
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef *AdcHandle)
{
    //set_led(0);
    adc__conversion_complete_callback();
}
#endif

void HAL_ADC_MspInit(ADC_HandleTypeDef *hadc)
{
    RCC_PeriphCLKInitTypeDef  PeriphClkInit;

    /* Enable clock of ADCx peripheral */
    __HAL_RCC_ADC1_CLK_ENABLE();

    /* Configure ADCx clock prescaler */
    /* Caution: On STM32F1, ADC clock frequency max is 14MHz (refer to device   */
    /*          datasheet).                                                     */
    /*          Therefore, ADC clock prescaler must be configured in function   */
    /*          of ADC clock source frequency to remain below this maximum      */
    /*          frequency.                                                      */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_ADC;
    PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;
    HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit);

#ifdef USE_DMA
    /* Enable clock of DMA associated to the peripheral */
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* Configure DMA parameters */
    AnalogDmaHandle.Instance = DMA1_Channel1;

    AnalogDmaHandle.Init.Direction           = DMA_PERIPH_TO_MEMORY;
    AnalogDmaHandle.Init.PeriphInc           = DMA_PINC_DISABLE;
    AnalogDmaHandle.Init.MemInc              = DMA_MINC_ENABLE;
    AnalogDmaHandle.Init.PeriphDataAlignment = DMA_PDATAALIGN_HALFWORD;   /* Transfer from ADC by half-word to match with ADC configuration: ADC resolution 10 or 12 bits */
    AnalogDmaHandle.Init.MemDataAlignment    = DMA_MDATAALIGN_HALFWORD;   /* Transfer to memory by half-word to match with buffer variable type: half-word */
    AnalogDmaHandle.Init.Mode                = DMA_NORMAL;//CIRCULAR;              /* DMA in circular mode to match with ADC configuration: DMA continuous requests */
    AnalogDmaHandle.Init.Priority            = DMA_PRIORITY_HIGH;

    /* Deinitialize  & Initialize the DMA for new transfer */
    HAL_DMA_DeInit(&AnalogDmaHandle);
    HAL_DMA_Init(&AnalogDmaHandle);

    /* Associate the initialized DMA handle to the ADC handle */
    __HAL_LINKDMA(hadc, DMA_Handle, AnalogDmaHandle);

    /*##-4- Configure the NVIC #################################################*/

    /* NVIC configuration for DMA interrupt (transfer completion or error) */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 1, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
#endif


    /* NVIC configuration for ADC interrupt */
    /* Priority: high-priority */
    HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(ADC1_IRQn);
    outstring("MSP init ADC\r\n");
}

uint16_t *adc__raw(void)
{
    return (uint16_t*)aADCxConvertedValues;
}