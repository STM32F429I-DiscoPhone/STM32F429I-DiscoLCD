#ifndef _SIM900A_H
#define _SIM900A_H

typedef struct {
    char manufacturer[32];
    char type[32];
    char imei[32];
} SIM900A_MODULE_T;

void SIM900A_Init();
void SIM900A_GetModule(SIM900A_MODULE_T *module);

void SIM900A_Dial(char *number);
void SIM900A_Answer()

void SIM900A_Test();
#endif
