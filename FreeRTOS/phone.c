#include "phone.h"

/* SIMCOM module */
#include "simcom.h"

enum State next;

void prvPhoneTask(void *pvParameters)
{
    static enum State current = MAIN;
    TickType_t xLastWakeTime;

    /* Initialize task and some utils */
    SIMCOM_Init();
    // Create the check incoming task
    xTaskCreate( prvIncomingTask, "Check incoming", configMINIMAL_STACK_SIZE * 2, NULL, mainCheck_TASK_PRIORITY, NULL);
    // Create the check button task
    xTaskCreate( prvButtonTask, "Check button", configMINIMAL_STACK_SIZE * 2, NULL, mainButton_TASK_PRIORITY, NULL);
    
    // Initialize the xLastWakeTime variable with current time.
    xLastWakeTime = xTaskGetTickCount();

    while(1) {
        vTaskDelayUntil(&xLastWakeTime, MAIN_TASK_DELAY);
        if(current == next) {
            continue;
        }

        current = next;
        switch(current) {
            // TODO : Main screen display
            case MAIN:
                break;
            // TODO : Incoming screen display
            case INCOMING:
                break;
            // TODO : During calling display
            case DURING:
                break;
            // TODO : Dial screen display
            case DIAL:
                break;
            // TODO : Send message screen display
            case SEND:
                break;
            // TODO : Read message screen display
            case READ:
                break;
            default:
                break;
        }
    }
}

void prvButtonTask(void *pvParameters)
{
    return;
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
