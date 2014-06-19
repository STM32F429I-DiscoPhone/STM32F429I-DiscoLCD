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
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"

/* uGFX includes. */
#include "gfx.h"


/* Task priorities. */
#define mainFLASH_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainLCD_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )
#define SLEEP_TICKS       30000/portTICK_PERIOD_MS
#define NEXT_CHAR_TICKS   5000/portTICK_PERIOD_MS

static GListener gl;


static TickType_t ticks_to_sleep = SLEEP_TICKS, ticks_to_nextchar = NEXT_CHAR_TICKS;
static TickType_t ticks_now, ticks_last, ticks_of_last_char;

static TaskHandle_t LCD_Handle;

static uint8_t locking = 0;

static const char_of_button[10][5] = { {' ','0',' ','0',0}, {',','.','!','?','1'}, {'a','b','c','2',0}, {'d','e','f','3',0}, {'g','h','i','4',0}, {'j','k','l','5',0}, {'m','n','o','6',0},{'p','q','r','s','7'},{'t','u','v','8',0},{'w','x','y','z','9'} };

int atoi(char *s)
{
	uint32_t num;
	num = 0;
	while (*s >= '0' && *s <= '9') {
		num = num * 10 + (*s - '0');
		s++;
	}
	return num;
}

char* itoa(uint32_t num)
{
	static char buffer[10 + 1];
	char *p = buffer + 10;
	if (num >= 0) {
		do {
			*(--p) = '0' + (num % 10);
			num /= 10;
		} while (num != 0);
	} else {
		do {
			*(--p) = '0' - (num % 10);
			num /= 10;
		} while (num != 0);
		*--p = '-';
	}
	return p;
}

static void prvSetupHardware(void)
{
	/* Setup STM32 system (clock, PLL and Flash configuration) */
	SystemInit();

	/* Ensure all priority bits are assigned as preemption priority bits. */
	NVIC_PriorityGroupConfig( NVIC_PriorityGroup_4 );

	/* Setup the LED outputs. */
	vParTestInitialise();
}

static GHandle  MainMenuContainer, KeypadContainer, CallContainer, MsgContainer, CallOutContainer, CallInContainer;

static GHandle	PHONEBtn, MSGBtn, CallBtn, CancelBtn, OneBtn, TwoBtn, ThreeBtn, FourBtn, FiveBtn, SixBtn, SevenBtn, EightBtn, NineBtn, StarBtn, ZeroBtn, JingBtn, AnswerBtn, DeclineBtn, HangoffBtn;
static GHandle  NumLabel, MsgLabel[5], IncomingLabel, OutgoingLabel;

void createsContainer(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = FALSE;

	// Containers
	wi.g.x = 0;
	wi.g.y = 10;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight();
	wi.text = "MM";
	MainMenuContainer = gwinContainerCreate(0, &wi, 0);
	
	wi.text = "In";
	CallInContainer = gwinContainerCreate(0, &wi, 0);

	wi.text = "Out";
	CallOutContainer = gwinContainerCreate(0, &wi, 0);

	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight()/2 - 20;
	wi.g.x = 0;
	wi.g.y = 10;
	wi.text = "CC";
	CallContainer = gwinContainerCreate(0, &wi, 0);

	wi.text = "Msg";
	MsgContainer = gwinContainerCreate(0, &wi, 0);

	wi.g.y = wi.g.height + 10;
	wi.g.height = gdispGetHeight() - wi.g.y;
	wi.text = "keypad";
	KeypadContainer = gwinContainerCreate(0, &wi, 0);
}

void createsKeypad(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.parent = KeypadContainer;
	// One button
	wi.g.x = 10;
	wi.g.y = 0;
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

void createsMsglabel(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;

	wi.g.parent = MsgContainer;
	// Message Label 0 (Up arrow)
	wi.g.x = 10;
	wi.g.width = gdispGetWidth() - 20;
	wi.g.y = 7;
	wi.g.height = 12;
	wi.text = "^^^";
	MsgLabel[0] = gwinLabelCreate(0, &wi);

	// Message Label 1
	wi.text = "";
	wi.g.y += wi.g.height;
	MsgLabel[1] = gwinLabelCreate(0, &wi);

	// Message Label 2
	wi.g.y += wi.g.height;
	MsgLabel[2] = gwinLabelCreate(0, &wi);

	// Message Label 3
	wi.g.y += wi.g.height;
	MsgLabel[3] = gwinLabelCreate(0, &wi);

	// Message Label 4 (Down arrow)
	wi.text = "vvv";
	wi.g.y += wi.g.height;
	MsgLabel[4] = gwinLabelCreate(0, &wi);
}

void createsMain(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.parent = MainMenuContainer;
	// Main Menu
	wi.g.show = TRUE;
	wi.g.x = 30;
	wi.g.y = 40;
	wi.g.width = gdispGetWidth() - 60;
	wi.g.height = 30;
	wi.text = "PHONE CALL";
	PHONEBtn = gwinButtonCreate(0, &wi);
	
	wi.g.y += wi.g.height + 3;
	wi.text = "SMS";
	MSGBtn = gwinButtonCreate(0, &wi);

}

void createsCall(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.parent = CallContainer;
	// Number label
	wi.g.width = gdispGetWidth() - 20;
	wi.g.height = 50;
	wi.g.y = 20;
	wi.g.x = 10;
	wi.text = "   ";
	NumLabel = gwinLabelCreate(0, &wi);

	// Call button
	wi.g.width = gdispGetWidth()/2 + 10;
	wi.g.height = 40;
	wi.g.y = gdispGetHeight()/2 - 60;
	wi.g.x = 10;
	wi.text = "CALL";
	CallBtn = gwinButtonCreate(0, &wi);
	
	// Cancel button
	wi.g.x += wi.g.width;
	wi.g.width = gdispGetWidth() - wi.g.x - 10;
	wi.text = "C";
	CancelBtn = gwinButtonCreate(0, &wi);

}

void createsIncall(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.parent = CallInContainer;

	// Answer Button
	wi.g.x = 10;
	wi.g.y = gdispGetHeight()/2 - 40;
	wi.g.width = gdispGetWidth()/2 - 20;
	wi.g.height = 80;
	wi.text = "ANSWER";
	AnswerBtn = gwinButtonCreate(0, &wi);

	// Decline Button
	wi.g.x += wi.g.width;
	wi.text = "DECLINE";
	DeclineBtn = gwinButtonCreate(0, &wi);

	// Incoming Number Show
	wi.g.x = 30;
	wi.g.y = 50;
	wi.g.height = 40;
	wi.g.width = gdispGetWidth() -60;
	wi.text = " ";
	IncomingLabel = gwinLabelCreate(0, &wi);
}

void createsOutcall(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;
	wi.g.parent = CallOutContainer;	

	// Hangoff Button
	wi.g.x = 10;
	wi.g.y = gdispGetHeight()/2 - 40;
	wi.g.width = gdispGetWidth() - 20;
	wi.g.height = 80;
	wi.text = "HANG OFF";
	HangoffBtn = gwinButtonCreate(0, &wi);
	// Outgoing Number Show
	wi.g.x = 30;
	wi.g.y = 50;
	wi.g.height = 40;
	wi.g.width = gdispGetWidth() -60;
	wi.text = " ";
	OutgoingLabel = gwinLabelCreate(0, &wi);
}

void incoming(char *num)
{
	gwinHide(MainMenuContainer);
	gwinHide(MsgContainer);
	gwinHide(CallContainer);
	gwinHide(KeypadContainer);
	gwinShow(CallInContainer);

	gwinSetText(IncomingLabel, num, TRUE);

	while (1) {
		//TODO: accept or reject call
	}
}

void outgoing(char* number)
{
	gwinHide(MainMenuContainer);
	gwinHide(MsgContainer);
	gwinHide(CallContainer);
	gwinHide(KeypadContainer);
	gwinShow(CallOutContainer);

	gwinSetText(OutgoingLabel, number, TRUE);
	
	//TODO: call out
	while (1) {
		//TODO: wait end call or reject call
	}
}

void createsUI(void)
{
	char *msg = "Demo";
	font_t		font1;
	
	font1 = gdispOpenFont("DejaVuSans24*");
	gdispClear(White);
	gdispDrawString(gdispGetWidth()-gdispGetStringWidth(msg, font1)-3, 3, msg, font1, Black);
	gdispCloseFont(font1);

	createsContainer();
	createsMsglabel();
	createsMain();
	createsCall();
	createsKeypad();
	createsIncall();
	createsOutcall();
}

void gotosleep(void)
{
	locking = 1;
	LCD_DisplayOff();
	vTaskSuspend( NULL );
}

void lockphone(void)
{
	LCD_DisplayOff();
	vTaskSuspend( LCD_Handle );
}

void wakeup(void)
{
	BaseType_t xYieldRequired;	
	LCD_DisplayOn();
	ticks_last = xTaskGetTickCount();
	ticks_to_sleep = SLEEP_TICKS;
	xYieldRequired = xTaskResumeFromISR( LCD_Handle );
	//taskYIELD();
	if (xYieldRequired == pdTRUE) {
		portYIELD_FROM_ISR(xYieldRequired);
	}
}


/* Find if the char in the button */
uint32_t char_in_button(char c, uint32_t btn)
{
	uint32_t i;
	for (i = 0; i < 5; i++) {
		if (c == char_of_button[btn][i]) {
			if (char_of_button[btn][i] != 0)
				return i;
			else
				return -1;
		}
	}
	return -1;
}

/* Phone UI & Control Task */
static void prvLCDTask(void *pvParameters)
{
	( void ) pvParameters;
	GEvent* pe;
	gfxInit();
	gwinAttachMouse(0);
	char labeltext[16] = "";
	char msgbuffer[255] = "";
	char linebuffer[3][31];
	char last_char = 0;
	char *last_char_in_lb;
	uint32_t textindex = 0, page = 0, msgindex = 0, currentline = 0, lineindex = 0, changing = 0, i;

	createsUI();
	geventListenerInit(&gl);
	gwinAttachListener(&gl);

	gwinShow(MainMenuContainer);

	page = 0;
 	
	ticks_to_sleep = SLEEP_TICKS;
	ticks_now = xTaskGetTickCount();
	ticks_last = ticks_now;
	ticks_of_last_char = 0;

	while (TRUE) {
		pe = geventEventWait(&gl, 5);
		ticks_now = xTaskGetTickCount();
		if (!pe) {
			if (ticks_to_sleep <= ticks_now - ticks_last) {
				gotosleep();
				ticks_to_sleep = SLEEP_TICKS;
				continue;
			} else {
				ticks_to_sleep -= (ticks_now - ticks_last);
			}
		} else {
			ticks_to_sleep = SLEEP_TICKS;
			switch(pe->type) {
				case GEVENT_GWIN_BUTTON:
					if (((GEventGWinButton*)pe)->button == PHONEBtn) {
						gwinHide(MainMenuContainer);
						gwinHide(MsgContainer);
						gwinShow(CallContainer);
						gwinShow(KeypadContainer);
						page = 1;
					} else if (((GEventGWinButton*)pe)->button == MSGBtn) {
						gwinHide(MainMenuContainer);
						gwinHide(CallContainer);
						gwinShow(MsgContainer);
						gwinShow(KeypadContainer);
						page = 2;
					} else if (((GEventGWinButton*)pe)->button == CallBtn) {
						outgoing(labeltext);
					} else if (((GEventGWinButton*)pe)->button == CancelBtn) {
						if (page == 1) {
							textindex = 0;
							labeltext[0] = '\0';
							changing = 1;
						}
					} else if (((GEventGWinButton*)pe)->button == OneBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '1';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 1) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 1) + 1;
								if (i > 4 || char_of_button[1][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[1][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[1][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[1][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[1][0];
							}
						}
					} else if (((GEventGWinButton*)pe)->button == TwoBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '2';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 2) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 2) + 1;
								if (i > 4 || char_of_button[2][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[2][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[2][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[2][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[2][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == ThreeBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '3';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 3) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 3) + 1;
								if (i > 4 || char_of_button[3][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[3][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[3][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[3][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[3][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == FourBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '4';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 4) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 4) + 1;
								if (i > 4 || char_of_button[4][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[4][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[4][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[4][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[4][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == FiveBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '5';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 5) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 5) + 1;
								if (i > 4 || char_of_button[5][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[5][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[5][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[5][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[5][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == SixBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '6';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 6) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 6) + 1;
								if (i > 4 || char_of_button[6][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[6][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[6][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[6][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[6][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == SevenBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '7';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 7) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 7) + 1;
								if (i > 4 || char_of_button[7][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[7][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[7][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[7][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[7][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == EightBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '8';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 8) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 8) + 1;
								if (i > 4 || char_of_button[8][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[8][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[8][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[8][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[8][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == NineBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '9';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 9) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 9) + 1;
								if (i > 4 || char_of_button[9][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[9][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[9][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[9][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[9][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == ZeroBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '0';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							if (last_char != 0 && char_in_button(last_char, 0) != -1 && (ticks_now-ticks_of_last_char <= NEXT_CHAR_TICKS)) {
								i = char_in_button(last_char, 0) + 1;
								if (i > 4 || char_of_button[0][i] == 0) {
									i = 0;
								}
								last_char = char_of_button[0][i];
								msgbuffer[msgindex - 1] = last_char;
								*last_char_in_lb = char_of_button[0][i];
							} else {
								msgbuffer[msgindex++] = char_of_button[0][0];
								last_char = msgbuffer[msgindex - 1];
								linebuffer[currentline][lineindex++] = char_of_button[0][0];
							}
							
						}
					} else if (((GEventGWinButton*)pe)->button == StarBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '*';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							msgbuffer[msgindex++] = '*';
							linebuffer[currentline][lineindex++] = '*';
						}
					} else if (((GEventGWinButton*)pe)->button == JingBtn) {
						changing = 1;
						if (page == 1) {
							labeltext[textindex++] = '#';
							labeltext[textindex] = '\0';
						} else if (page == 2) {
							msgbuffer[msgindex++] = '#';
							linebuffer[currentline][lineindex++] = '#';
						}
					}
					break;
				default:
					break;
			}
			if (changing == 1 && page == 1) {
				gwinSetText(NumLabel, labeltext, TRUE);
				changing = 0;
			} else if (changing == 1 && page == 2) {
				changing = 0;
				msgbuffer[msgindex] = '\0';
				linebuffer[currentline][lineindex] = '\0';
				ticks_of_last_char = ticks_now;

				if (lineindex > 0)
					last_char_in_lb = &(linebuffer[currentline][lineindex-1]);
				else if (currentline != 0)
					last_char_in_lb = &(linebuffer[currentline-1][0]);

				gwinSetText(MsgLabel[1], linebuffer[0], TRUE);
				gwinSetText(MsgLabel[2], linebuffer[1], TRUE);
				gwinSetText(MsgLabel[3], linebuffer[2], TRUE);
				if (lineindex == 31) {
					if (currentline != 2) {
						currentline ++;
						lineindex = 0;
					} else {
						//TODO: overline
					}
				}
			}
		}
		ticks_last = ticks_now;
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
	if (EXTI_GetITStatus(EXTI_Line0) != RESET) {
		if (locking == 0) {
			locking = 1;
			lockphone();
		} else {
			locking = 0;
			wakeup();
		}
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
	/* Start the LCD task */
	xTaskCreate( prvLCDTask, "LCD", configMINIMAL_STACK_SIZE * 2, NULL, mainLCD_TASK_PRIORITY, &LCD_Handle );

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
