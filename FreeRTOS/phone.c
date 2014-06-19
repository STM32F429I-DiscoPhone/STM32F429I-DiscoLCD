#include "phone.h"

enum State next;

void prvPhoneTask(void *pvParameters)
{
    static enum State current = MAIN;

    while(1) {
        vTaskDelay(MAIN_TASK_DELAY);
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
            // TODO : Error display
            case ERROR:
                break;
            default:
        }
    }
}

void prvButtonTask(void *pvParameters)
{

}

void prvIncomingTask(void *pvParameters)
{

}
