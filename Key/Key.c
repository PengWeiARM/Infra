#define KEY_GLOBLS

#include "key.h"

u16 wLedCnt = 0;
static keyInit_t *pApp;

void sInitKey(keyInit_t *pInit)
{
    pApp = pInit;
}

void RstLed(void)
{
    wLedCnt = 60000;
}

void sReadkey(void)	   //1ms
{
#define cKeyFilter	20
	static u16 wKeyCnt[5]={0,0,0,0,0};
	static u16 wLongPress[5]={0,0,0,0,0};
	if(wLedCnt>0)
	{
		Back_Led_On();
		wLedCnt--;
	}
	else
	{
	 	Back_Led_Off();
	}

	if((!Key_Up)&&(!Key_Down))
	{
		wKeyCnt[4]++;
		if(wKeyCnt[4] == cKeyFilter)
		{
			if(wLedCnt!=0)
			{
                pApp->pSendKeyEvent(eKeyEntEsc);
			}

		}
		RstLed();
	}
	else
	{
		wKeyCnt[4] = 0;
	}
	if(!Key_Enter)
	{
		wKeyCnt[0]++;
		if(wKeyCnt[0] == cKeyFilter)
		{
			if(wLedCnt!=0)
			{
                pApp->pSendKeyEvent(eKeyEnt);
			}
		}
	   	RstLed();
	}
	else
	{
		wKeyCnt[0] = 0;
	}

	if(!Key_Up)
	{
		wKeyCnt[1]++;
		if(wKeyCnt[1] == cKeyFilter)
		{
			if(wLedCnt!=0)
			{
                pApp->pSendKeyEvent(eKeyUp);
			}
		}
		RstLed();
		if(wKeyCnt[1] > 20*cKeyFilter)
		{
			wKeyCnt[1] = 20*cKeyFilter;
			if(wLongPress[1]++>	15*cKeyFilter)
			{
			   wLongPress[1] = 0;
               pApp->pSendKeyEvent(eKeyUpLong);
			}
		}
	}
	else
	{
		wKeyCnt[1] = 0;
	}

	if(!Key_Down)
	{
		wKeyCnt[2]++;
		if(wKeyCnt[2] == cKeyFilter)
		{
			if(wLedCnt!=0)
			{
                pApp->pSendKeyEvent(eKeyDown);
			}
		}

		RstLed();

		if(wKeyCnt[2] > 20*cKeyFilter)
		{
			wKeyCnt[2] = 20*cKeyFilter;
			if(wLongPress[2]++>	15*cKeyFilter)
			{
			   wLongPress[2] = 0;
               pApp->pSendKeyEvent(eKeyDownLong);
			}
		}
	}
	else
	{
		wKeyCnt[2] = 0;
	}

	if(!Key_Esc)
	{
		RstLed();
		wKeyCnt[3]++;
		if(wKeyCnt[3] == cKeyFilter)
		{
			if(wLedCnt!=0)
			{
                pApp->pSendKeyEvent(eKeyEsc);
			}
		}
		if(wKeyCnt[3] > 20*cKeyFilter)
		{
			wKeyCnt[3] = 20*cKeyFilter;
			if(wLongPress[3]++>	15*cKeyFilter)
			{
			   wLongPress[3] = 0;
               pApp->pSendKeyEvent(eKeyEsc);
			}
		}
	}
	else
	{
		wKeyCnt[3] = 0;
	}
}
