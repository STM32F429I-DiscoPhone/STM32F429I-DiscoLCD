#ifndef _MYLIB_H
#define _MYLIB_H

/* uGFX includes. */
#include "gfx.h"

#define SLEEP_TICKS       300000/portTICK_PERIOD_MS
#define NEXT_CHAR_TICKS   5000/portTICK_PERIOD_MS

// SMS CHAR IN each button
static const char_of_button[10][5] = { {' ','0',' ','0',0}, {',','.','!','?','1'}, {'a','b','c','2',0}, {'d','e','f','3',0}, {'g','h','i','4',0}, {'j','k','l','5',0}, {'m','n','o','6',0},{'p','q','r','s','7'},{'t','u','v','8',0},{'w','x','y','z','9'} };



void createsUI(void);
void createsContainer(void);
void createsKeypad(void);
void createsReadMsg(void);
void createsWriteMsg(void);
void createsMain(void);
void createsCall(void);
void createsIncall(void);
void createsOutcall(void);
void hideall(void);

void calling(char *number);

uint32_t char_in_button(char c, uint32_t btn);
#endif
