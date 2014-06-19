#ifndef _PHONE_H
#define _PHONE_H

#include <FreeRTOS.h>
#include <task.h>

/* Task priority */
#define mainPhone_TASK_PRIORITY				( tskIDLE_PRIORITY + 1 )
#define mainButton_TASK_PRIORITY			( tskIDLE_PRIORITY + 2 )
#define mainCheck_TASK_PRIORITY				( tskIDLE_PRIORITY + 2 )

/* Main task delay period */
#define MAIN_TASK_DELAY (1000 / portTICK_PERIOD_MS)

/* Incoming call checking delay period */
#define INCOMING_TASK_DELAY (10000 / portTICK_PERIOD_MS)

/* State Number
 * 0 : Main
 * 1 : Call incoming
 * 2 : During call
 * 3 : Dial keypad
 * 4 : Send SMS
 * 5 : Read SMS
 */
enum State {MAIN, INCOMING, DURING, DIAL, SEND, READ};

/* Phone main task */
void prvPhoneTask(void *pvParameters);

/* Touch panel event waiting task */
void prvButtonTask(void *pvParameters);

/* Check incoming call task */
void prvIncomingTask(void *pvParameters);

#endif
