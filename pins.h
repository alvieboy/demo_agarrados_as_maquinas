#ifndef __PINS_H__
#define __PINS_H__


#define PORT_A 0
#define PORT_B 1
#define PORT_C 2
#define PORT_D 3

#define MAKEPIN(port, pin) ((pin|(port<<6)))

typedef uint8_t pin_t;

static inline GPIO_TypeDef *pin__port(pin_t pin)
{
    switch(pin>>6) {
    case 0: return GPIOA;
    case 1: return GPIOB;
    case 2: return GPIOC;
    case 3: return GPIOD;
    }
    return NULL;
}

static inline uint32_t pin__pin(pin_t pin)
{
    return (1<<(pin&0x1F));
}

static inline void configure_inputs(pin_t *pins, unsigned len)
{
    unsigned int i;
    GPIO_InitTypeDef init;
    init.Mode = GPIO_MODE_INPUT;
    init.Pull = GPIO_PULLUP;
    init.Speed = GPIO_SPEED_FREQ_LOW;

    for (i=0;i<len; i++) {
        pin_t pin = pins[i];
        GPIO_TypeDef *port = pin__port(pin);
        init.Pin = pin__pin(pin);
        HAL_GPIO_Init( port, &init );
    }
}


#endif
