#include "distance.h"
#include "defs.h"
#include <stm32f1xx_hal_gpio.h>
#include <stm32f1xx_hal.h>
#include "uart.h"
#include "debug.h"

#define DISTANCE_THRESHOLD 50000 // discard everything above 50ms this gives ~15meters
#define DIST_ARRAY_SIZE 5
#define METERS_PER_TICK 0.000314f

static TIM_HandleTypeDef    TimHandle;
static TIM_IC_InitTypeDef ICHandle;



uint32_t distance[DIST_ARRAY_SIZE] = {0};

#if 0
static void distance_test_init()
{

//    #define DIST_TEST
    
    GPIO_InitTypeDef init_pb12;

    init_pb12.Pin = GPIO_PIN_12;
    init_pb12.Mode = GPIO_MODE_OUTPUT_PP;
    init_pb12.Pull = GPIO_NOPULL;
    init_pb12.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOB, &init_pb12 );
    
    GPIO_InitTypeDef init_pb13;

    init_pb13.Pin = GPIO_PIN_13;
    init_pb13.Mode = GPIO_MODE_OUTPUT_PP;
    init_pb13.Pull = GPIO_NOPULL;
    init_pb13.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOB, &init_pb13 );
    
    GPIO_InitTypeDef init_pb14;

    init_pb14.Pin = GPIO_PIN_14;
    init_pb14.Mode = GPIO_MODE_OUTPUT_PP;
    init_pb14.Pull = GPIO_NOPULL;
    init_pb14.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOB, &init_pb14 );
    
    GPIO_InitTypeDef init_pb15;

    init_pb15.Pin = GPIO_PIN_15;
    init_pb15.Mode = GPIO_MODE_OUTPUT_PP;
    init_pb15.Pull = GPIO_NOPULL;
    init_pb15.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( GPIOB, &init_pb15 );
    
    
}
int distance_read_echo()
{
    return HAL_GPIO_ReadPin( DISTANCE_ECHO_GPIO, DISTANCE_ECHO_GPIO_PIN );

}
#endif

void distance_init()
{
    /*Trigger GPIO init*/

    GPIO_InitTypeDef init;

    DISTANCE_TRIG_GPIO_CLK_ENABLE();
    
    DISTANCE_TIM_CLK_ENABLE();
    
    DISTANCE_TIM_CHANNEL_GPIO_PORT();

    init.Pin = DISTANCE_TRIG_GPIO_PIN;
    init.Mode = GPIO_MODE_OUTPUT_PP;
    init.Pull = GPIO_NOPULL;
    init.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( DISTANCE_TRIG_GPIO, &init );
    
    GPIO_InitTypeDef GPIOinit;

    GPIOinit.Pin = DISTANCE_ECHO_GPIO_PIN | DISTANCE_ECHO2_GPIO_PIN;
    GPIOinit.Mode = GPIO_MODE_AF_INPUT;
    GPIOinit.Pull = GPIO_NOPULL;
    GPIOinit.Speed = GPIO_SPEED_FREQ_HIGH;

    HAL_GPIO_Init( DISTANCE_ECHO_GPIO, &GPIOinit );

    TimHandle.Instance = DISTANCE_TIM;
    TimHandle.Init.Prescaler         = 36*10;
    TimHandle.Init.Period            = 0xFFFF;
    TimHandle.Init.ClockDivision     = 0;
    TimHandle.Init.CounterMode       = TIM_COUNTERMODE_UP;
    TimHandle.Init.RepetitionCounter = 0;
    
    
    // Timer 3 channel 1 input capture configuration
    ICHandle.ICPolarity =  TIM_ICPOLARITY_FALLING;     //Capture a rising edge
    ICHandle.ICSelection = TIM_ICSELECTION_DIRECTTI;   // => IC1
    ICHandle.ICPrescaler = 0 ;
    ICHandle.ICFilter = 0;                          //If there is noise during the beging of capture specify number of rising edges to ignore max of 0xF

    //HAL_TIM_ConfigTI1Input(TIM_HandleTypeDef * htim, uint32_t TI1_Selection)

    HAL_TIM_IC_Init(&TimHandle);

    //HAL_TIM_SelectSlaveMode(DISTANCE_TIM, TIM_SlaveMode_Reset);

    HAL_TIM_IC_ConfigChannel(&TimHandle, &ICHandle, DISTANCE_TIM_CHANNEL);  // => PA6

    // Timer 3 channel 1 input capture configuration
    ICHandle.ICPolarity =  TIM_ICPOLARITY_RISING;     //Capture a rising edge
    ICHandle.ICSelection = TIM_ICSELECTION_DIRECTTI;   // => IC1
    ICHandle.ICPrescaler = 0 ;
    ICHandle.ICFilter = 0;                          //If there is noise during the beging of capture specify number of rising edges to ignore max of 0xF

    HAL_TIM_IC_ConfigChannel(&TimHandle, &ICHandle, DISTANCE_TIM_CHANNEL2);  // => PA6


    __enable_irq();

    HAL_NVIC_SetPriority(DISTANCE_TIM_IRQ, 27, 0);
    NVIC_EnableIRQ( DISTANCE_TIM_IRQ );

    return;
}






static void distance_delay()
{
//    HAL_Delay(100);
    __disable_irq();
    volatile int counter = 15000;
    while (counter--) {
    }
    __enable_irq();
}

void distance_ping()
{
    //Stop timer in input capture mode TIM3
    HAL_TIM_IC_Stop(&TimHandle, DISTANCE_TIM_CHANNEL);
    HAL_TIM_IC_Stop(&TimHandle, DISTANCE_TIM_CHANNEL2);

    //Start timer in input capture mode
    HAL_TIM_IC_Start_IT(&TimHandle, DISTANCE_TIM_CHANNEL);
    HAL_TIM_IC_Start_IT(&TimHandle, DISTANCE_TIM_CHANNEL2);

    DISTANCE_TIM->CNT=0;
    HAL_GPIO_WritePin( DISTANCE_TRIG_GPIO, DISTANCE_TRIG_GPIO_PIN, 1);
    
    distance_delay();

    HAL_GPIO_WritePin( DISTANCE_TRIG_GPIO, DISTANCE_TRIG_GPIO_PIN, 0);
}

int distance_read(uint32_t *value, uint8_t *closeness)
{
    uint32_t dist = 0;
    int i = 0;

    for(i = 0 ;i < DIST_ARRAY_SIZE; i++)
        dist += distance[i];
        
    *value = dist;//(uint32_t)(((float)dist/(float)DIST_ARRAY_SIZE)*METERS_PER_TICK*0.5*100);
    
#ifdef DIST_TEST
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, 0);
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, 0);
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, 0);
    HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, 0);
#endif

    if(*value > 0 && *value < 50)
    {
        *closeness = DISTANCE_UPCLOSE;
#ifdef DIST_TEST
        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_12, 1);
#endif
    }else if(*value < 100)
    {
        *closeness = DISTANCE_CLOSE;
#ifdef DIST_TEST
        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_13, 1);
#endif
    }else if(*value < 200)
    {
        *closeness = DISTANCE_FAR;
#ifdef DIST_TEST
        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_14, 1);
#endif
    }else
    {
        *closeness = DISTANCE_NODETECTION;
#ifdef DIST_TEST
        HAL_GPIO_WritePin( GPIOB, GPIO_PIN_15, 1);
#endif
    }
    return 0;
}

uint8_t distance_meas_idx = 0;
unsigned count;
unsigned count_seq=0;

unsigned distance_get_seq(void)
{
    return count_seq;
}

unsigned rise_count, fall_count;

unsigned distance_get_count(void)
{
    debug__printf("RISE %d FALL %d\n", rise_count, fall_count);
    if (rise_count>fall_count)
    {
        return 0xFFFF-(fall_count - rise_count);
    }
    return fall_count - rise_count;
}

void distance_echo_interrupt()
{
    HAL_TIM_IRQHandler(&TimHandle);
}


void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Channel==DISTANCE_TIM_CHANNEL2) {
        // Rising
        rise_count = 0xdead;
    } else {
        fall_count = HAL_TIM_ReadCapturedValue(htim, DISTANCE_TIM_CHANNEL);
        rise_count = HAL_TIM_ReadCapturedValue(htim, DISTANCE_TIM_CHANNEL2);
        // Falling
    }
    count_seq++;
}
#if 0
void distance_data(void)
{
    //outstring("IT\r\n");
    //Stop timer in input capture mode TIM3
    //count = DISTANCE_TIM->CNT;
    count = DISTANCE_TIM->CCR2;
    //count = DISTANCE_TIM->CCR1;
  //  HAL_TIM_IC_Stop(&TimHandle, DISTANCE_TIM_CHANNEL);
    count_seq++;
//    DISTANCE_TIM->SR &= ~(TIM_SR_UIF);

    __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC2);
    __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC1);
    __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC3);
    __HAL_TIM_CLEAR_IT(&TimHandle, TIM_IT_CC4);
#if 0
    if(DISTANCE_TIM->SR & TIM_SR_UIF)
    {
        if(DISTANCE_TIM->CNT < DISTANCE_THRESHOLD)
            distance[distance_meas_idx] = DISTANCE_TIM->CNT;
            distance_meas_idx += 1;
            
            if(distance_meas_idx > DIST_ARRAY_SIZE) 
                distance_meas_idx = 0;
        
        DISTANCE_TIM->SR &= ~(TIM_SR_UIF);
    }
#endif
}
#endif
