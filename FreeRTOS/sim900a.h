#ifndef _SIM900A_H
#define _SIM900A_H

typedef struct SMS_STRUCT_T {
    char number[16];
    char time[32];
    char content[64];
} SMS_STRUCT;

void SIM900A_Init();

void SIM900A_Dial(char *number);
void SIM900A_Answer();
void SIM900A_HangUp();

void SIM900A_Test();
#endif
