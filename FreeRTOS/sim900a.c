#include "sim900a.h"
#include <stm32f4xx.h>
#include <string.h>
#include "tm_stm32f4_usart.h"

#include <FreeRTOS.h>
#include <task.h>

#define DBG

#define WAITTIME 2000000

#ifdef DBG
static void dbg_puts(char *str)
{
    TM_USART_Puts(USART6, str);
}
#else
static void dbg_puts(char *str)
{

}
#endif

static void RecvResponse(char *response)
{
    int wait = WAITTIME;
    int recv = 0;
    char c;

    while(wait--) {
        if((c = TM_USART_Getc(USART1)) != 0) {
            if(c == '\n' || c == '\r') {

            }
            else {
                response[recv++] = c;
            }
        }
    }

    response[recv] = '\0';
}

/*
 * Send AT command.
 */
static void SendCmd(char *cmd)
{
    dbg_puts("Send: ");
    dbg_puts(cmd);
    dbg_puts("\n\r");

    TM_USART_Puts(USART1, cmd);
    TM_USART_Puts(USART1, "\r\n");
}

/*
 * Send AT command to SIM900A and check response immediately.
 */
static int SendCmd_Check(char *cmd, char *check)
{
    int result = 1;
    char recv_str[32];

    dbg_puts("Send: ");
    dbg_puts(cmd);
    dbg_puts("\n\r");
    TM_USART_Puts(USART1, cmd);
    TM_USART_Puts(USART1, "\r\n");

    if(check) {
        RecvResponse(recv_str);

        if(strlen(recv_str) > 0) {
            dbg_puts("Recv: ");
            dbg_puts(recv_str);
            dbg_puts("\n\r");
        }

        if(!strstr(recv_str, check)) {
            result = 0;
        }
    }

    return result;
}


void SIM900A_Init()
{
    /* Init USART communication between STM and SIM900A */
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);

#ifdef DBG
    /* Init USART Communication between PC and STM, use for debug */
    TM_USART_Init(USART6, TM_USART_PinsPack_1, 115200);
#endif

    while(1) {
        dbg_puts("Try initilize communication between STM32 & SIM900A\n\r");
        if(SendCmd_Check("AT", "OK")) {
            dbg_puts("Initilization successful!\n\r");
            break;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }

    while(1) {
        dbg_puts("Try close ATE function\n\r");
        if(SendCmd_Check("ATE0", "OK")) {
            dbg_puts("Close successful!\n\r");
            break;
        }

        vTaskDelay(5000 / portTICK_PERIOD_MS);
    }
}

void SIM900A_GetModule(SIM900A_MODULE_T *module)
{
    char recv[32];

    SendCmd("AT+CGMI");
    RecvResponse(recv);
    if(strlen(recv) > 0) {
        strncpy(module->manufacturer, recv, strlen(recv) - 2);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SendCmd("AT+CGMM");
    RecvResponse(recv);
    if(strlen(recv) > 0) {
        strncpy(module->type, recv, strlen(recv) - 2);
    }
    vTaskDelay(2000 / portTICK_PERIOD_MS);

    SendCmd("AT+CGSN");
    RecvResponse(recv);
    if(strlen(recv) > 0) {
        strncpy(module->imei, recv, strlen(recv) - 2);
    }
}

void SIM900A_Dial(char *number)
{
    char cmd[16] = {0};
    char recv[64];

    strcpy(cmd, "ATD");
    strcat(cmd, number);
    strcat(cmd, ";");

    dbg_puts("Dial to ");
    dbg_puts(number);
    dbg_puts("\n\r");
    SendCmd(cmd);
    while(1) {
        RecvResponse(recv);

        if(strlen(recv) == 0) {
            continue;
        }

        if(strstr(recv, "NO CARRIER")) {
            dbg_puts("NO CARRIER\n\r");
        }
        else if(strstr(recv, "NO ANSWER")) {
            dbg_puts("NO ANSWER\n\r");
        }
        else if(strstr(recv, "ERROR")) {
            dbg_puts("ERROR\n\r");
        }
        else if(strstr(recv, "OK")) {
            dbg_puts("Dial successful!\n\r");
        }

        break;
    }
}

/* Test API */
void SIM900A_Test()
{
    while(1) {
        vTaskDelay(5000 / portTICK_PERIOD_MS);
        SIM900A_MODULE_T module;
        SIM900A_GetModule(&module);

        dbg_puts(module.manufacturer);
        dbg_puts(", ");
        dbg_puts(module.type);
        dbg_puts(", ");
        dbg_puts(module.imei);
        dbg_puts("\n\r");
    }
}
