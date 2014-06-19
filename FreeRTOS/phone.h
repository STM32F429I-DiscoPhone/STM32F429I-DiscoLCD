#ifndef _PHONE_H
#define _PHONE_H

#include "task.h"

/* Main task delay period */
#define MAIN_TASK_DELAY (1000 / portTICK_PERIOD_MS)

/* State Number
 * 0 : Main
 * 1 : Call incoming
 * 2 : During call
 * 3 : Dial keypad
 * 4 : Send SMS
 * 5 : Read SMS
 * 6 : Error
 */
enum State {MAIN, INCOMING, DURING, DIAL, SEND, READ, ERROR};

/* Phone main task */
void prvPhoneTask(void *pvParameters);

/* Touch panel event waiting task */
void prvButtonTask(void *pvParameters);

/* Check incoming call task */
void prvIncomingTask(void *pvParameters)

#endif
