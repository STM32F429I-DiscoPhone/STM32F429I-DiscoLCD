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

/* Task priorities. */
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )

static void prvSetupHardware(void)
{
    /* Setup STM32 system (clock, PLL and Flash configuration) */
    SystemInit();

    /* Ensure all priority bits are assigned as preemption priority bits. */
    NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

    /* Setup the LED outputs. */
    vParTestInitialise();
}

int main(void)
{
	/* Configure the hardware ready to run the test. */
	prvSetupHardware();

	/* Start standard demo/test application flash tasks. */
	vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );
	/* Start the Phone task */
	xTaskCreate( prvPhoneTask, "Phone", configMINIMAL_STACK_SIZE * 2, NULL, mainPhone_TASK_PRIORITY, NULL);

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
