#include "mylib.h"
/* SIM900 includes */
#include "simcom.h"
#include "phone.h"

extern enum State next;
extern GListener gl;

// Container Handle
GHandle  MainMenuContainer, KeypadContainer, CallContainer, MsgContainer, CallOutContainer, CallInContainer, ReadMsgContainer;

// Button & Label Handle
GHandle	RETURNBtn, PHONEBtn, READSMSBtn, WRITESMSBtn, CallBtn, CancelBtn, OneBtn, TwoBtn, ThreeBtn, FourBtn, FiveBtn, SixBtn, SevenBtn, EightBtn, NineBtn, StarBtn, ZeroBtn, JingBtn, AnswerBtn, DeclineBtn, HangoffBtn, BackspaceBtn, SendBtn, SwapBtn;
GHandle  NumLabel, MsgLabel[3], TargetLabel, IncomingLabel, OutgoingLabel, ReadMsgLabel[10];
uint32_t char_in_button(char c, uint32_t btn)
{
	uint32_t i;
	for (i = 0; i < 5; i++) {
		if ( char_of_button[btn][i] == 0)
			return -1;
		else if(c == char_of_button[btn][i])
			return i;
	}
	return -1;
}

void createsContainer(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = FALSE;

	// Containers
	wi.g.x = 0;
	wi.g.y = 15;
	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight() - 20;
	wi.text = "MM";
	MainMenuContainer = gwinContainerCreate(0, &wi, 0);

	wi.text = "Read SMS";
	ReadMsgContainer = gwinContainerCreate(0, &wi, 0);
	
	wi.text = "In";
	CallInContainer = gwinContainerCreate(0, &wi, 0);

	wi.text = "Out";
	CallOutContainer = gwinContainerCreate(0, &wi, 0);

	wi.g.width = gdispGetWidth();
	wi.g.height = gdispGetHeight()/2 - 20;
	wi.g.x = 0;
	wi.g.y = 15;
	wi.text = "CC";
	CallContainer = gwinContainerCreate(0, &wi, 0);

	wi.text = "Write SMS";
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

void createsReadMsg(void)
{
	uint8_t i;
	GWidgetInit wi;
	gwinWidgetClearInit(&wi);
	wi.g.show = TRUE;

	wi.g.parent = ReadMsgContainer;
	wi.g.x = 10;
	wi.g.y = 30;
	wi.g.height = 20;
	wi.g.width = gdispGetWidth() - 20;
	wi.text = "";
	// label for 0 ~ 9
	for (i = 0; i < 10; i++) {
		ReadMsgLabel[i] = gwinLabelCreate(0, &wi);
		wi.g.y += wi.g.height;
	}
}

void createsWriteMsg(void)
{
	GWidgetInit wi;
	gwinSetDefaultFont(gdispOpenFont("UI2"));
	gwinSetDefaultStyle(&WhiteWidgetStyle, FALSE);
	gwinWidgetClearInit(&wi);

	wi.g.show = TRUE;

	wi.g.parent = MsgContainer;
	// SMS Target
	wi.g.x = 10;
	wi.g.width = gdispGetWidth() - 50;
	wi.g.y = 10;
	wi.g.height = 12;
	wi.text = "";
	TargetLabel = gwinLabelCreate(0, &wi);

	// Message Label 1
	wi.g.x = 10;
	wi.g.y = 30;
	wi.text = "";
	wi.g.y += wi.g.height;
	MsgLabel[0] = gwinLabelCreate(0, &wi);

	// Message Label 2
	wi.g.y += wi.g.height;
	MsgLabel[1] = gwinLabelCreate(0, &wi);

	// Message Label 3
	wi.g.y += wi.g.height;
	MsgLabel[2] = gwinLabelCreate(0, &wi);

	// Swap button
	wi.g.x = gdispGetWidth() - 40;
	wi.g.y = 23;
	wi.g.width = 40;
	wi.g.height = 15;
	wi.text = "SWAP";
	SwapBtn = gwinButtonCreate(0, &wi);

	// Send button
	wi.g.width = gdispGetWidth()/2 - 10;
	wi.g.height = 40;
	wi.g.y = gdispGetHeight()/2 - 60;
	wi.g.x = 10;
	wi.text = "SEND";
	SendBtn = gwinButtonCreate(0, &wi);

	// Backspace button
	wi.g.x += wi.g.width;
	wi.text = "<---";
	BackspaceBtn = gwinButtonCreate(0, &wi);
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
	wi.g.x = 30;
	wi.g.y = 40;
	wi.g.width = gdispGetWidth() - 60;
	wi.g.height = 30;
	wi.text = "PHONE CALL";
	PHONEBtn = gwinButtonCreate(0, &wi);
	
	wi.g.y += wi.g.height + 3;
	wi.text = "WRITE SMS";
	WRITESMSBtn = gwinButtonCreate(0, &wi);

	wi.g.y += wi.g.height + 3;
	wi.text = "READ SMS";
	READSMSBtn = gwinButtonCreate(0, &wi);
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

void hideall(void)
{
	gwinHide(MainMenuContainer);
	gwinHide(MsgContainer);
	gwinHide(CallContainer);
	gwinHide(CallInContainer);
	gwinHide(KeypadContainer);
	gwinHide(CallOutContainer);
	gwinHide(ReadMsgContainer);
}

void createsUI(void)
{
	char *msg = "My Phone";
	GWidgetInit wi;
	font_t		font1;
	
	font1 = gdispOpenFont("DejaVuSans24*");
	gdispClear(White);
	gdispDrawString(gdispGetWidth()-gdispGetStringWidth(msg, font1)-3, 3, msg, font1, Black);
	gdispCloseFont(font1);
	gwinWidgetClearInit(&wi);

	// RETURN TO TOP
	wi.g.show = TRUE;
	wi.g.x = 5;
	wi.g.y = 5;
	wi.g.width = 40;
	wi.g.height = 20;
	wi.text = "R";
	//RETURNBtn = gwinButtonCreate(0, &wi);

	createsContainer();
	createsWriteMsg();
	createsMain();
	createsReadMsg();
	createsCall();
	createsKeypad();
	createsIncall();
	createsOutcall();
}

void calling(char *number)
{
	GEvent* pe;

	gwinSetText(OutgoingLabel, number, TRUE);
	while (1) {
		pe = geventEventWait(&gl, TIME_INFINITE);
		switch(pe->type) {
			case GEVENT_GWIN_BUTTON:
				if (((GEventGWinButton*)pe)->button == HangoffBtn) {
					SIMCOM_HangUp();
					next = MAIN;
					return;
				}
				break;
			default:
				break;
		}
	}

}
