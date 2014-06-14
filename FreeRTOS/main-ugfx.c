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

/* Demo application includes. */
#include "partest.h"
#include "flash.h"

/* Hardware and starter kit includes. */
#include "stm32f4xx.h"

/* uGFX includes. */
#include "gfx.h"

/* SIM900 includes */
#include "sim900a.h"


/* Task priorities. */
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainLCD_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define mainAPI_TASK_PRIORITY				( tskIDLE_PRIORITY + 3 )

static GListener gl;

static void prvSetupHardware(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Setup the LED outputs. */
	vParTestInitialise();
}
static GHandle	CallBtn, CancelBtn, OneBtn, TwoBtn, ThreeBtn, FourBtn, FiveBtn, SixBtn, SevenBtn, EightBtn, NineBtn, StarBtn, ZeroBtn, JingBtn;
static GHandle  NumLabel;
void createsKeyPad(void)
{
	char *msg = "Demo";
	font_t		font1;
	GWidgetInit wi;
	
	font1 = gdispOpenFont("DejaVuSans24*");
	gdispClear(White);
	gdispDrawString(gdispGetWidth()-gdispGetStringWidth(msg, font1)-3, 3, msg, font1, Black);
	gdispCloseFont(font1);

	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;
	
	// Number label
	wi.g.width = gdispGetWidth() - 20;
	wi.g.height = 60;
	wi.g.y = 20;
	wi.g.x = 10;
	wi.text = "   ";
	NumLabel = gwinLabelCreate(0, &wi);

	// Call button
	wi.g.width = gdispGetWidth()/2 + 10;
	wi.g.height = 40;
	wi.g.y = gdispGetHeight()/2 - 50;
	wi.g.x = 10;
	wi.text = "CALL";
	CallBtn = gwinButtonCreate(0, &wi);
	
	// Cancel button
	wi.g.x += wi.g.width;
	wi.g.width = gdispGetWidth() - wi.g.x - 10;
	wi.text = "C";
	CancelBtn = gwinButtonCreate(0, &wi);

	// One button
	wi.g.x = 10;
	wi.g.y += wi.g.height;
	wi.g.width = ( gdispGetWidth() - 20 ) / 3;
	wi.g.height = (gdispGetHeight()/2)/4;
	wi.text = "1";
	OneBtn = gwinButtonCreate(0, &wi);

	// Two button
	wi.g.x += wi.g.width;
	wi.text = "2";
	TwoBtn = gwinButtonCreate(0, &wi);

	// Three button
	wi.g.x += wi.g.width;
	wi.text = "3";
	ThreeBtn = gwinButtonCreate(0, &wi);

	// Four button
	wi.g.x = 10;
	wi.g.y += wi.g.height;
	wi.text = "4";
	FourBtn = gwinButtonCreate(0, &wi);

	// Five button
	wi.g.x += wi.g.width;
	wi.text = "5";
	FiveBtn = gwinButtonCreate(0, &wi);

	// Six button
	wi.g.x += wi.g.width;
	wi.text = "6";
	SixBtn = gwinButtonCreate(0, &wi);

	// Seven button
	wi.g.x = 10;
	wi.g.y += wi.g.height;
	wi.text = "7";
	SevenBtn = gwinButtonCreate(0, &wi);

	// Eight button
	wi.g.x += wi.g.width;
	wi.text = "8";
	EightBtn = gwinButtonCreate(0, &wi);

	// Nine button
	wi.g.x += wi.g.width;
	wi.text = "9";
	NineBtn = gwinButtonCreate(0, &wi);

	// * button
	wi.g.x = 10;
	wi.g.y += wi.g.height;
	wi.text = "*";
	StarBtn = gwinButtonCreate(0, &wi);

	// Zero button
	wi.g.x += wi.g.width;
	wi.text = "0";
	ZeroBtn = gwinButtonCreate(0, &wi);

	// # button
	wi.g.x += wi.g.width;
	wi.text = "#";
	JingBtn = gwinButtonCreate(0, &wi);
}

/* GFX notepad demo */
static void prvLCDTask(void *pvParameters)
{
	( void ) pvParameters;
	GEvent* pe;
	gfxInit();
	gwinAttachMouse(0);
	char labeltext[16] = "";
	uint32_t textindex = 0;
	createsKeyPad();
	geventListenerInit(&gl);
	gwinAttachListener(&gl);
	while (TRUE) {
		pe = geventEventWait(&gl, TIME_INFINITE);

		switch(pe->type) {
			case GEVENT_GWIN_BUTTON:
				if (((GEventGWinButton*)pe)->button == CallBtn) {
					//TODO: call
				} else if (((GEventGWinButton*)pe)->button == CancelBtn) {
					textindex = 0;
					labeltext[0] = '\0';
				} else if (((GEventGWinButton*)pe)->button == OneBtn) {
					labeltext[textindex++] = '1';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == TwoBtn) {
					labeltext[textindex++] = '2';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == ThreeBtn) {
					labeltext[textindex++] = '3';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == FourBtn) {
					labeltext[textindex++] = '4';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == FiveBtn) {
					labeltext[textindex++] = '5';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == SixBtn) {
					labeltext[textindex++] = '6';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == SevenBtn) {
					labeltext[textindex++] = '7';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == EightBtn) {
					labeltext[textindex++] = '8';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == NineBtn) {
					labeltext[textindex++] = '9';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == ZeroBtn) {
					labeltext[textindex++] = '0';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == StarBtn) {
					labeltext[textindex++] = '*';
					labeltext[textindex] = '\0';
				} else if (((GEventGWinButton*)pe)->button == JingBtn) {
					labeltext[textindex++] = '#';
					labeltext[textindex] = '\0';
				}
				break;
			default:
				break;
		}
		gwinSetText(NumLabel, labeltext, TRUE);
	}
}

/* Test SIM900 API */
static void prvAPITest(void *pvParameters)
{
    SIM900A_Init();

    while(1) {
        SIM900A_Test();
    }
}

int main(void)
{
	/* Configure the hardware ready to run the test. */
	prvSetupHardware();

	/* Start standard demo/test application flash tasks. */
	vStartLEDFlashTasks( mainFLASH_TASK_PRIORITY );

	/* Start the LCD task */
	xTaskCreate( prvLCDTask, "LCD", configMINIMAL_STACK_SIZE * 2, NULL, mainLCD_TASK_PRIORITY, NULL );

	/* Start the SIM900A API test task */
	xTaskCreate( prvAPITest, "APITest", configMINIMAL_STACK_SIZE * 2, NULL, mainFLASH_TASK_PRIORITY, NULL );

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
