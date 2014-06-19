#include "phone.h"

/* SIMCOM module */
#include "simcom.h"

/* mylib include */
#include "mylib.h"

// Event Listener
GListener gl;

// Utility Tisk Variable
TickType_t ticks_to_sleep = SLEEP_TICKS, ticks_to_nextchar = NEXT_CHAR_TICKS;
TickType_t ticks_now, ticks_last, ticks_of_last_char;

// Container Handle
extern GHandle  MainMenuContainer, KeypadContainer, CallContainer, MsgContainer, CallOutContainer, CallInContainer, ReadMsgContainer;

// Button & Label Handle
extern GHandle	RETURNBtn, PHONEBtn, READSMSBtn, WRITESMSBtn, CallBtn, CancelBtn, OneBtn, TwoBtn, ThreeBtn, FourBtn, FiveBtn, SixBtn, SevenBtn, EightBtn, NineBtn, StarBtn, ZeroBtn, JingBtn, AnswerBtn, DeclineBtn, HangoffBtn, BackspaceBtn, SendBtn, SwapBtn;
extern GHandle  NumLabel, MsgLabel[3], TargetLabel, IncomingLabel, OutgoingLabel, ReadMsgLabel[10];

extern uint8_t locking;

enum State next, current;

void prvPhoneTask(void *pvParameters)
{
    current = MAIN;
	next = MAIN;
    TickType_t xLastWakeTime;

    /* Initialize task and some utils */
    SIMCOM_Init();
	gfxInit();
	gwinAttachMouse(0);
	createsUI();
	gwinShow(MainMenuContainer);
    // Create the check incoming task
    xTaskCreate( prvIncomingTask, "Check incoming", configMINIMAL_STACK_SIZE * 2, NULL, mainCheck_TASK_PRIORITY, NULL);
    // Create the check button task
    xTaskCreate( prvButtonTask, "Check button", configMINIMAL_STACK_SIZE * 2, NULL, mainButton_TASK_PRIORITY, &LCD_Handle);
    
    // Initialize the xLastWakeTime variable with current time.
    xLastWakeTime = xTaskGetTickCount();
	ticks_last = xLastWakeTime;
    while(1) {
        vTaskDelayUntil(&xLastWakeTime, MAIN_TASK_DELAY);
		ticks_now = xTaskGetTickCount();
	//	if ( ticks_to_sleep < (ticks_now - ticks_last)) {
			//LCD_DisplayOff();
			//locking = 1;
			//vTaskSuspend( LCD_Handle );
			//vTaskSuspend( NULL );
	//	} else {
	//		ticks_to_sleep -= (ticks_now - ticks_last);
	//	}
        if(current == next) {
            continue;
        }

        current = next;
		hideall();
        switch(current) {
            // TODO : Main screen display
            case MAIN:
				gwinShow(MainMenuContainer);
                break;
            // TODO : Incoming screen display
            case INCOMING:
				gwinShow(CallInContainer);
                break;
            // TODO : During calling display
            case DURING:
				gwinShow(CallOutContainer);
                break;
            // TODO : Dial screen display
            case DIAL:
				gwinShow(CallContainer);
				gwinShow(KeypadContainer);
                break;
            // TODO : Send message screen display
            case SEND:
				gwinShow(MsgContainer);
				gwinShow(KeypadContainer);
                break;
            // TODO : Read message screen display
            case READ:
				gwinShow(ReadMsgContainer);
                break;
            default:
				break;
        }
		ticks_last = ticks_now;
    }
}

void prvButtonTask(void *pvParameters)
{
	GEvent* pe;
	SMS_STRUCT sms[3];

	char labeltext[16] = "";
	char msgbuffer[255] = "";
	char numbuffer[16] = "";
	char linebuffer[3][31];
	char last_char = 0;
	char *last_char_in_lb;
	uint32_t textindex = 0, msgindex = 0, currentline = 0, lineindex = 0, numindex = 0, changing = 0, msgORnum = 0, i, j, totalmsg, readline = 0;

	geventListenerInit(&gl);
	gwinAttachListener(&gl);
	
	ticks_to_sleep = SLEEP_TICKS;
	ticks_now = xTaskGetTickCount();
	ticks_of_last_char = 0;

	while (TRUE) {
		pe = geventEventWait(&gl, TIME_INFINITE); // wait forever, so if screen not touched, the task will be blocked
		ticks_now = xTaskGetTickCount();
		ticks_to_sleep = SLEEP_TICKS;
		switch(pe->type) {
			case GEVENT_GWIN_BUTTON:
				if (((GEventGWinButton*)pe)->button == PHONEBtn) {
					// change status to DIAL
					next = DIAL;
				} else if (((GEventGWinButton*)pe)->button == RETURNBtn) {
					next = MAIN;
				} else if (((GEventGWinButton*)pe)->button == WRITESMSBtn) {
					// change status to READ
					next = SEND;
					msgbuffer[0] = '\0';
					msgindex = 0;
					linebuffer[0][0] = '\0';
					linebuffer[1][0] = '\0';
					linebuffer[2][0] = '\0';
					currentline = 0;
					lineindex = 0;
					numbuffer[0] = '\0';
					numindex = 0;
				} else if (((GEventGWinButton*)pe)->button == READSMSBtn) {
					next = READ;
					//TODO: read sms here
					linebuffer[0][0] = '\0';
					lineindex = 0;
					totalmsg = SIMCOM_ReadSMS(sms);
					if (totalmsg == 0) {
						gwinSetText(ReadMsgLabel[4], "NO MSG AVAILABLE", TRUE);
						continue;
					}
					readline = 0;
					currentline = 0;
					for (i = 0; i < totalmsg; i++) {
						/* FROM: [number] */
						strcpy(linebuffer[0], "FROM: ");
						strcat(linebuffer[0], sms[i].number);
						gwinSetText(ReadMsgLabel[readline++], linebuffer[0], TRUE);
						/* Message */
						strcpy(linebuffer[0], sms[i].content);
						gwinSetText(ReadMsgLabel[readline++], linebuffer[0], TRUE);
					}
					
				} else if (((GEventGWinButton*)pe)->button == CallBtn) {
					// change status to dial and call out
					next = DURING;
					SIMCOM_Dial(labeltext);
					calling(labeltext);
				} else if (((GEventGWinButton*)pe)->button == CancelBtn) {
					// Clear Number Label
					textindex = 0;
					labeltext[0] = '\0';
					changing = 1;
				} else if (((GEventGWinButton*)pe)->button == AnswerBtn) {
					SIMCOM_Answer();
					next = DURING;
					calling("UNKNOWN");
				} else if (((GEventGWinButton*)pe)->button == DeclineBtn) {
					SIMCOM_HangUp();
					next = MAIN;
				} else if (((GEventGWinButton*)pe)->button == SendBtn) {
					// TODO: send message
					SIMCOM_SendSMS(numbuffer, msgbuffer);
					next = MAIN;
				} else if (((GEventGWinButton*)pe)->button == BackspaceBtn) {
					if (msgORnum == 0) {
						if (msgindex == 0)
							continue;
						changing = 1;
						last_char = 0;
						msgindex--;
						*last_char_in_lb = '\0';
						if (lineindex == 0) {
							lineindex = 30;
							currentline--;
						} else {
							lineindex--;
						}
					} else {
						if (numindex == 0)
							continue;
						changing = 1;
						numindex--;
					}
				} else if (((GEventGWinButton*)pe)->button == SwapBtn) {
					if (msgORnum == 0)
						msgORnum = 1;
					else
						msgORnum = 0;
				} else if (((GEventGWinButton*)pe)->button == OneBtn) {
					changing = 1;
					if (current == DIAL) { // if current is dial UI
						labeltext[textindex++] = '1';
						labeltext[textindex] = '\0';
					} else if (current == SEND) { //if current is SEND MSG UI
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '1';
						}
					}
				} else if (((GEventGWinButton*)pe)->button == TwoBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '2';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '2';
						}
					}
				} else if (((GEventGWinButton*)pe)->button == ThreeBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '3';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '3';
						}
					}
				} else if (((GEventGWinButton*)pe)->button == FourBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '4';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '4';
						}
						
					}
				} else if (((GEventGWinButton*)pe)->button == FiveBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '5';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '5';
						}
						
					}
				} else if (((GEventGWinButton*)pe)->button == SixBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '6';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '6';
						}
							
					}
				} else if (((GEventGWinButton*)pe)->button == SevenBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '7';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '7';
						}
							
					}
				} else if (((GEventGWinButton*)pe)->button == EightBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '8';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '8';
						}
						
					}
				} else if (((GEventGWinButton*)pe)->button == NineBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '9';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '9';
						}
						
					}
				} else if (((GEventGWinButton*)pe)->button == ZeroBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '0';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
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
						} else { //write to number
							numbuffer[numindex++] = '0';
						}
					}
				} else if (((GEventGWinButton*)pe)->button == StarBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '*';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
							msgbuffer[msgindex++] = '*';
							linebuffer[currentline][lineindex++] = '*';
						} else { //write to number
							numbuffer[numindex++] = '*';
						}
					}
				} else if (((GEventGWinButton*)pe)->button == JingBtn) {
					changing = 1;
					if (current == DIAL) {
						labeltext[textindex++] = '#';
						labeltext[textindex] = '\0';
					} else if (current == SEND) {
						if (msgORnum == 0) { // write to msg
							msgbuffer[msgindex++] = '#';
							linebuffer[currentline][lineindex++] = '#';
						} else { //write to number
							numbuffer[numindex++] = '#';
						}
					}
				}
				break;
			default:
				break;
		}
		if (changing == 1) {
			if (current == DIAL) {
				gwinSetText(NumLabel, labeltext, TRUE);
				changing = 0;
			} else if (current == SEND) {
				if (msgORnum == 0) { // write to msg
					changing = 0;
					msgbuffer[msgindex] = '\0';
					linebuffer[currentline][lineindex] = '\0';
					ticks_of_last_char = ticks_now;

					if (lineindex > 0)
						last_char_in_lb = &(linebuffer[currentline][lineindex-1]);
					else if (currentline != 0)
						last_char_in_lb = &(linebuffer[currentline-1][0]);

					gwinSetText(MsgLabel[0], linebuffer[0], TRUE);
					gwinSetText(MsgLabel[1], linebuffer[1], TRUE);
					gwinSetText(MsgLabel[2], linebuffer[2], TRUE);
					if (lineindex == 31) {
						if (currentline != 2) {
							currentline ++;
							lineindex = 0;
						} else {
							//TODO: overline
						}
					}
				} else { //write to number
					changing = 0;
					numbuffer[numindex] = '\0';
					gwinSetText(TargetLabel, numbuffer, TRUE);
				}
			}
		}
	}
}

/* This task is used for checking incoming call.
 * Because the SIMCOM module doesn't have interrupt
 * when calling income. So we check manually every
 * time period.
 */
void prvIncomingTask(void *pvParameters)
{
    TickType_t xLastWakeTime;

    // Initialize the xLastWakeTime variable with current time.
    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, INCOMING_TASK_DELAY);

        if(SIMCOM_CheckPhone()) {
            next = INCOMING;
        }
    }
}
