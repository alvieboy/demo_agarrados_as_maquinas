
TARGET=efeitos

all: $(TARGET).bin

CROSS=arm-none-eabi-
CC=$(CROSS)gcc
AS=$(CROSS)gcc
OBJCOPY=$(CROSS)objcopy
SIZE=$(CROSS)size

CUBE=../../STM32Cube_FW_F1_V1.4.0/

HAL=$(CUBE)/Drivers/STM32F1xx_HAL_Driver/

HALSRCPATH=$(HAL)/Src
HALINCPATH=$(HAL)/Inc

USBDEVICE=$(CUBE)/Middlewares/ST/STM32_USB_Device_Library/
USBCOREINC=$(USBDEVICE)/Core/Inc
USBCORESRC=$(USBDEVICE)/Core/Src

USB_SOURCES=\
	usbd_core.c  \
	usbd_ctlreq.c  \
	usbd_ioreq.c

USBCDCINC=$(USBDEVICE)/Class/CDC/Inc
USBCDCSRC=$(USBDEVICE)/Class/CDC/Src

USBCDC_SOURCES=usbd_cdc.c

HAL_SOURCES= \
stm32f1xx_hal_adc.c                 \
stm32f1xx_hal_adc_ex.c              \
stm32f1xx_hal.c                     \
stm32f1xx_hal_can.c                 \
stm32f1xx_hal_cec.c                 \
stm32f1xx_hal_cortex.c              \
stm32f1xx_hal_crc.c                 \
stm32f1xx_hal_dac.c                 \
stm32f1xx_hal_dac_ex.c  	    \
stm32f1xx_hal_dma.c                 \
stm32f1xx_hal_eth.c                 \
stm32f1xx_hal_flash.c               \
stm32f1xx_hal_flash_ex.c            \
stm32f1xx_hal_gpio.c                \
stm32f1xx_hal_gpio_ex.c             \
stm32f1xx_hal_hcd.c                 \
stm32f1xx_hal_i2c.c                 \
stm32f1xx_hal_i2s.c                 \
stm32f1xx_hal_irda.c                \
stm32f1xx_hal_iwdg.c                \
stm32f1xx_hal_nand.c                \
stm32f1xx_hal_nor.c                 \
stm32f1xx_hal_pccard.c              \
stm32f1xx_hal_pcd.c                 \
stm32f1xx_hal_pcd_ex.c              \
stm32f1xx_hal_pwr.c                 \
stm32f1xx_hal_rcc.c                 \
stm32f1xx_hal_rcc_ex.c              \
stm32f1xx_hal_rtc.c                 \
stm32f1xx_hal_rtc_ex.c              \
stm32f1xx_hal_sd.c                  \
stm32f1xx_hal_smartcard.c           \
stm32f1xx_hal_spi.c                 \
stm32f1xx_hal_spi_ex.c              \
stm32f1xx_hal_sram.c                \
stm32f1xx_hal_tim.c                 \
stm32f1xx_hal_tim_ex.c              \
stm32f1xx_hal_uart.c                \
stm32f1xx_hal_usart.c               \
stm32f1xx_hal_wwdg.c                \
stm32f1xx_ll_usb.c              

_HOBJ=$(HAL_SOURCES:.c=.o)
_USBOBJ=$(USB_SOURCES:.c=.o)
_USBCDCOBJ=$(USBCDC_SOURCES:.c=.o)

HOBJ=$(patsubst %, $(HALSRCPATH)/%, $(_HOBJ))
USBOBJ=$(patsubst %, $(USBCORESRC)/%, $(_USBOBJ))
USBCDCOBJ=$(patsubst %, $(USBCDCSRC)/%, $(_USBCDCOBJ))

CFLAGS= -std=c11 -O2 -Wmissing-prototypes -Wall -Werror -Wno-error=unused-function -mcpu=cortex-m3 -mthumb -mthumb-interwork -I$(HALINCPATH) -I. \
-I$(CUBE)/Drivers/CMSIS/Device/ST/STM32F1xx/Include/ \
-I$(CUBE)/Drivers/CMSIS/Include \
-I$(USBCOREINC) \
-I$(USBCDCINC) \
-DSTM32F103xB \
-fdata-sections \
-ffunction-sections

ASFLAGS=-O2 -mcpu=cortex-m3 -mthumb -x assembler-with-cpp

XSRC=psu.c

SRC=main.c \
    setup.c \
    uart.c \
    spi.c \
    lcd_oled.c \
    delay.c \
    font.c \
    cpu.c \
    adc.c \
    hsv.c \
    strip.c \
    fft.c \
    fixed.c \
    test_signal.c \
    decay_accel.c \
    effect.c \
    effect_classic.c \
    effect_decay.c \
    effect_hsv.c \
    effect_move1.c \
    effect_move2.c \
    effect_fft.c \
    vsnprintf.c \
    system_stm32f1xx.c  \
    stm32f1xx_it.c  

 
ASRC=startup_stm32f103x6.s

AOBJ=$(ASRC:.s=.o)
OBJS=$(SRC:.c=.o) $(AOBJ)



$(TARGET).bin: $(TARGET).elf
	$(OBJCOPY) -O binary $< $@

$(TARGET).elf: $(OBJS) $(HOBJ) 
	$(CC) -o $@.dyn $+ $(CFLAGS) -shared -nostartfiles
	$(CC) $(CFLAGS) -o $@ $+ -Tstm32f103.ld -Wl,--gc-sections -nostartfiles -static -flto
	$(SIZE) $@

$(AOBJ) : %.o : %.s
	$(CC) -c $(ASFLAGS) $< -o $@

clean:
	rm -f $(OBJS) $(HOBJ) $(USBOBJ) $(USBCDCOBJ) $(TARGET).elf

flash: $(TARGET).bin
	sudo dfu-util -d 1eaf:0003 -c 1 -i 0 -a 2 -D $(TARGET).bin

