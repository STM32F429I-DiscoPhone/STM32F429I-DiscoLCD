/*
 * Copyright (c) 2012, 2013, Joel Bodenmann aka Tectu <joel@unormal.org>
 * Copyright (c) 2012, 2013, Andrew Hannam aka inmarket
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *    * Redistributions of source code must retain the above copyright
 *      notice, this list of conditions and the following disclaimer.
 *    * Redistributions in binary form must reproduce the above copyright
 *      notice, this list of conditions and the following disclaimer in the
 *      documentation and/or other materials provided with the distribution.
 *    * Neither the name of the <organization> nor the
 *      names of its contributors may be used to endorse or promote products
 *      derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL <COPYRIGHT HOLDER> BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/* Kernel includes. */
#include "FreeRTOS.h"

/* Hardware and starter kit includes */
#include "stm32f4xx.h"

/* Demo application includes. */
#include "partest.h"
#include "flash.h"

/* uGFX includes. */
#include "gfx.h"

/* Tasks */
#include "phone.h"
#include "mylib.h"
#include "simcom.h"

/* Task priorities. */
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )

static void prvSetupHardware(void)
{
    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /* Setup the LED outputs. */
    vParTestInitialise();
}

uint8_t locking = 0;

extern TickType_t ticks_now, ticks_last, ticks_to_sleep;
extern TaskHandle_t Phone_Handle;
extern TaskHandle_t LCD_Handle;
extern enum State next, current;

void lockphone(void)
{
	if (current == MAIN) {
		LCD_DisplayOff();
		locking = 1;
		vTaskSuspend( Phone_Handle );
		vTaskSuspend( LCD_Handle );
	} else if (current != DURING && current != INCOMING) {
		next = MAIN;
	}
}

void wakeup(void)
{
	BaseType_t xYieldRequired;	
	locking = 0;
	LCD_DisplayOn();
	ticks_last = xTaskGetTickCount();
	ticks_to_sleep = SLEEP_TICKS;
	xYieldRequired = xTaskResumeFromISR( Phone_Handle );
	xYieldRequired = xTaskResumeFromISR( LCD_Handle );
	//taskYIELD();
	if (xYieldRequired == pdTRUE) {
		portYIELD_FROM_ISR(xYieldRequired);
	}
}

/* Set button as interrupt */
void initButton(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef gpio_init;
	GPIO_StructInit(&gpio_init);
	gpio_init.GPIO_Mode = GPIO_Mode_IN;
	gpio_init.GPIO_PuPd = GPIO_PuPd_NOPULL;
	gpio_init.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init(GPIOA, &gpio_init);

	EXTI_InitTypeDef exti_init;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA,EXTI_PinSource0);
	exti_init.EXTI_Line = EXTI_Line0;
	exti_init.EXTI_Mode = EXTI_Mode_Interrupt;
	exti_init.EXTI_Trigger = EXTI_Trigger_Rising;
	exti_init.EXTI_LineCmd = ENABLE;
    EXTI_Init(&exti_init);

	NVIC_InitTypeDef nvic_init;
	nvic_init.NVIC_IRQChannel = EXTI0_IRQn;
	nvic_init.NVIC_IRQChannelPreemptionPriority = 0x0F;
	nvic_init.NVIC_IRQChannelSubPriority = 0x0F;
	nvic_init.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&nvic_init);
}

void EXTI0_IRQHandler(void)
{
	uint32_t i;
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		if (locking == 0) {
			lockphone();
		} else {
			wakeup();
		}
		i = 100000;
		while (i > 0)
			i--;
		
		EXTI_ClearITPendingBit(EXTI_Line0);
	}
}

int main(void)
{
	/* Configure the hardware ready to run the test. */
	prvSetupHardware();

	initButton();
	/* Start standard demo/test application flash tasks. */
	vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
	/* Start the Phone task */
	xTaskCreate( prvPhoneTask, "Phone", configMINIMAL_STACK_SIZE * 2, NULL, mainPhone_TASK_PRIORITY, &Phone_Handle);

	/* Start the scheduler. */
	vTaskStartScheduler();
}

void vApplicationStackOverflowHook(TaskHandle_t pxTask, char *pcTaskName)
{
	(void)pxTask;
	(void)pcTaskName;

	for (;;);
}

void vApplicationTickHook(void)
{
}
