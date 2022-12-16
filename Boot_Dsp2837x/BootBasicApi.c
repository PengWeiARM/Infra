// ##########################################################################################
#include "BootBasicApi.h"
// ##########################################################################################


// ##########################################################################################
void    voTimeoutByStep();
void    voSetTimeoutTarget(int32_ta* pTimer,int32_ta ms);
bool_ta boIsTargetTimout(int32_ta* pTimer);
void    voStopTimer(int32_ta* pTimer);
// ##########################################################################################
int32_ta s32TimeIntervalSet = 10;
int32_ta s32BootTimerCntTick = 0;
// ##########################################################################################
//
//
//
// ##########################################################################################
void voTimeoutByStep()
{
    s32BootTimerCntTick++;
}


void  voSetTimeoutTarget(int32_ta* pTimer,int32_ta ms) {
	  int cnt = ms/(s32TimeIntervalSet);
    if(cnt == 0) {
        cnt = 1;
    }
    *pTimer = s32BootTimerCntTick + cnt;
}

void  voStartTimerMs(int32_ta* pTimer,int32_ta ms) {
	voSetTimeoutTarget(pTimer,ms);
}


bool_ta boIsTargetTimout(int32_ta* pTimer) {
   return (*pTimer > s32TimeIntervalSet);
}


void voStopTimer(int32_ta* pTimer)
{
     *pTimer = s32BootTimerCntTick;
}

