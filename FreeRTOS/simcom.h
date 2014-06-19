#ifndef _SIMCOM_H
#define _SIMCOM_H

typedef struct SMS_STRUCT_T {
    char number[16];
    char content[64];
} SMS_STRUCT;

void SIMCOM_Init();

void SIMCOM_Dial(char *number);
void SIMCOM_Answer();
void SIMCOM_HangUp();

int SIMCOM_ReadSMS(SMS_STRUCT sms[3]);

void SIMCOM_Test();
#endif
