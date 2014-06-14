#include "sim900.h"
#include <stm32f4xx.h>
#include <string.h>
#include "tm_stm32f4_usart.h"

#define WAITTIME 20000000

static int RecvResponse(char *response)
{
    int wait = WAITTIME;
    int recv = 0;
    char c;

    while(wait--) {
        if((c = TM_USART_Getc(USART1)) != 0) {
            response[recv++] = c;
        }
    }

    response[recv] = '\0';

    return recv;
}

static int SendCmd_Check(char *cmd, char *check)
{
    int result = 1;
    char recv_str[32];

    TM_USART_Puts(USART6, "Send: ");
    TM_USART_Puts(USART6, cmd);
    TM_USART_Puts(USART6, "\n\r");
    TM_USART_Puts(USART1, cmd);

    if(check) {
        int recv = RecvResponse(recv_str);

        if(recv > 0) {
            TM_USART_Puts(USART6, "Recv: ");
            TM_USART_Puts(USART6, recv_str);
            TM_USART_Puts(USART6, "\n\r");
        }
        if(strcmp(recv_str, check)) {
            result = 0;
        }
    }

    return result;
}

void SIM900_Init()
{
    TM_USART_Init(USART1, TM_USART_PinsPack_1, 115200);
    TM_USART_Init(USART6, TM_USART_PinsPack_1, 115200);
}

void SIM900_Test()
{
    while(1) {
        /*
        char c = TM_USART_Getc(USART6);
        if(c) {
            TM_USART_Puts(USART6, "Get: ");
            TM_USART_Putc(USART6, c);
            TM_USART_Putc(USART6, '\n');
            TM_USART_Putc(USART6, '\r');
        }
        */

        SendCmd_Check("AT", "OK");
    }
}
