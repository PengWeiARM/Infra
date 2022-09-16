#ifndef KEY_H
#define KEY_H

#ifndef KEY_GLOBLS
#define EXT extern 
#else
#define EXT
#endif
#include "typedef.h"
#include "macro.h"

typedef struct
{
    void (*pSendKeyEvent)(u32 id);
}keyInit_t;


#define Key_Enter		GPIO_KEY_ENTER
#define Key_Down		GPIO_KEY_DOWN
#define Key_Up			GPIO_KEY_UP
#define Key_Esc			GPIO_KEY_ESC

#define Back_Led_On()   GPIO_Back_Led_On()
#define Back_Led_Off()   GPIO_Back_Led_Off()


void sInitKey(keyInit_t *pInit);
void sReadkey(void);
void RstLed(void);

#endif
