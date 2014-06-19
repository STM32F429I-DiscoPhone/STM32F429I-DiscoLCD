PROJECT = rtos800

EXECUTABLE = $(PROJECT).elf
BIN_IMAGE = $(PROJECT).bin
HEX_IMAGE = $(PROJECT).hex

# set the path to STM32F429I-Discovery firmware package
STDP ?= ../../STM32F429I-Discovery_FW_V1.0.1

# set the path to FreeRTOS package
RTOS ?= ../../FreeRTOSv8.0.1

# set the path to uGFX package
UGFX ?= ../../ugfx

# Toolchain configurations
CROSS_COMPILE ?= arm-none-eabi-
CC = $(CROSS_COMPILE)gcc
LD = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
OBJDUMP = $(CROSS_COMPILE)objdump
SIZE = $(CROSS_COMPILE)size

# Cortex-M4 implements the ARMv7E-M architecture
CPU = cortex-m4
CFLAGS = -mcpu=$(CPU) -march=armv7e-m -mtune=cortex-m4
CFLAGS += -mlittle-endian -mthumb

# FPU
CFLAGS += -mfpu=fpv4-sp-d16 -mfloat-abi=softfp

# Libraries
LIBS = -lc -lnosys
LDFLAGS =
define get_library_path
    $(shell dirname $(shell $(CC) $(CFLAGS) -print-file-name=$(1)))
endef
LDFLAGS += -L $(call get_library_path,libc.a)
LDFLAGS += -L $(call get_library_path,libgcc.a)

# Basic configurations
CFLAGS += -g -std=c99 -Wall

# Optimizations
CFLAGS += -O3 -ffast-math
CFLAGS += -ffunction-sections -fdata-sections
CFLAGS += -Wl,--gc-sections
CFLAGS += -fno-common
CFLAGS += --param max-inline-insns-single=1000

# specify STM32F429
CFLAGS += -DSTM32F429_439xx

# to run from FLASH
LDFLAGS += -T stm32f429zi_flash.ld

# Project source
CFLAGS += -I.
UGFX_OBJS = \
    gdisp_lld_ILI9341.o \
    ginput_lld_mouse.o \
    main-ugfx.o \
    simcom.o \
    phone.o \
    tm_stm32f4_usart.o \
    ParTest.o \
    port.o \
    timertest.o \
    system_stm32f4xx.o

# Startup file
UGFX_OBJS += startup_stm32f429_439xx.o

# CMSIS
CFLAGS += -I$(STDP)/Libraries/CMSIS/Device/ST/STM32F4xx/Include
CFLAGS += -I$(STDP)/Libraries/CMSIS/Include

# STM32F4xx_StdPeriph_Driver
CFLAGS += -DUSE_STDPERIPH_DRIVER
CFLAGS += -I$(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/inc
UGFX_OBJS += \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/misc.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_dma2d.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_fmc.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_i2c.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_ltdc.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_gpio.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_usart.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_rcc.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_exti.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_syscfg.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_spi.o \
    $(STDP)/Libraries/STM32F4xx_StdPeriph_Driver/src/stm32f4xx_tim.o

# STM32F429I-Discovery Utilities
CFLAGS += -I$(STDP)/Utilities/STM32F429I-Discovery
CFLAGS += -I$(STDP)/Utilities/Common
UGFX_OBJS += \
    $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_ioe.o \
    $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_lcd.o \
    $(STDP)/Utilities/STM32F429I-Discovery/stm32f429i_discovery_sdram.o

# FreeRTOS
CFLAGS += -I $(RTOS)/FreeRTOS/Source/include
CFLAGS += -I$(RTOS)/FreeRTOS/Source/portable/GCC/ARM_CM4F
CFLAGS += -I$(RTOS)/FreeRTOS/Demo/Common/include
UGFX_OBJS += \
    $(RTOS)/FreeRTOS/Source/list.o \
    $(RTOS)/FreeRTOS/Source/queue.o \
    $(RTOS)/FreeRTOS/Source/tasks.o \
    $(RTOS)/FreeRTOS/Source/timers.o \
    $(RTOS)/FreeRTOS/Source/portable/MemMang/heap_1.o \
    $(RTOS)/FreeRTOS/Demo/Common/Minimal/flash.o

# uGFX
CFLAGS += -I$(UGFX)
CFLAGS += -I$(UGFX)/src/gdisp/mcufont
UGFX_OBJS += \
    $(UGFX)/src/gfx.o \
    $(UGFX)/src/gdisp/fonts.o \
    $(UGFX)/src/gdisp/gdisp.o \
    $(UGFX)/src/gdisp/mcufont/mf_encoding.o \
    $(UGFX)/src/gdisp/mcufont/mf_font.o \
    $(UGFX)/src/gdisp/mcufont/mf_justify.o \
    $(UGFX)/src/gdisp/mcufont/mf_scaledfont.o \
    $(UGFX)/src/gdisp/mcufont/mf_rlefont.o \
    $(UGFX)/src/gevent/gevent.o \
    $(UGFX)/src/ginput/ginput.o \
    $(UGFX)/src/ginput/mouse.o \
    $(UGFX)/src/gos/freertos.o \
    $(UGFX)/src/gtimer/gtimer.o \
    $(UGFX)/src/gwin/console.o \
    $(UGFX)/src/gwin/gwin.o \
	$(UGFX)/src/gwin/gwm.o \
	$(UGFX)/src/gwin/gwidget.o \
	$(UGFX)/src/gwin/button.o \
	$(UGFX)/src/gwin/label.o \
	$(UGFX)/src/gwin/gcontainer.o \
	$(UGFX)/src/gqueue/gqueue.o

ugfx: $(UGFX_OBJS)
ugfx: CFLAGS += -DGFX_USE_OS_FREERTOS=TRUE -DGFX_NOTEPAD_DEMO=TRUE
ugfx: OBJS = $(UGFX_OBJS)
ugfx: $(BIN_IMAGE)

all: ugfx

$(BIN_IMAGE): $(EXECUTABLE)
	$(OBJCOPY) -O binary $^ $@
	$(OBJCOPY) -O ihex $^ $(HEX_IMAGE)
	$(OBJDUMP) -h -S -D $(EXECUTABLE) > $(PROJECT).lst
	$(SIZE) $(EXECUTABLE)

$(EXECUTABLE): $(OBJS)
	$(LD) -o $@ $(OBJS) \
		--start-group $(LIBS) --end-group \
		$(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.S
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(SIMPLE_LED_OBJS) $(COMPLEX_LED_OBJS) $(LCD_OBJS) $(UGFX_BASIC_OBJS) $(UGFX_OBJS)
	rm -f $(EXECUTABLE) $(BIN_IMAGE) $(HEX_IMAGE)
	rm -f $(PROJECT).lst

flash:
	st-flash write $(BIN_IMAGE) 0x8000000

.PHONY: clean led-test simple-led complex-led
