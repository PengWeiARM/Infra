// ##########################################################################################
#include "BootBasicApi.h"
// ##########################################################################################


// ##########################################################################################
void    voTimerTick       (int32_ta* p32tick, bool_ta*  pstate);
void    voSetTimeoutTarget(int32_ta* pTimer , int32_ta* p32tick,int32_ta ms);
bool_ta boIsTargetTimout  (int32_ta* pTimer,int32_ta* p32tick);
void    voStopTimer       (int32_ta* pTimer,int32_ta* p32tick,bool_ta* pstate);
// ##########################################################################################
int32_ta s32TimeIntervalSet = mBinUpdateInterval;  // 10
int32_ta s32BootTimerCntTick = 0;
bool_ta  boTimerRunState = false;
// ##########################################################################################
//
//
//
// ##########################################################################################
void voTimeIntervalSet(int32_ta interval) {
	s32TimeIntervalSet = interval;
}

void voTimerTick(int32_ta* p32tick,bool_ta* pstate)
{
	  if(*pstate) {
			(*p32tick)= (*p32tick) + 1;
		} else {
			*p32tick = 0;
		}
}


void  voSetTimeoutTarget(int32_ta* pTimer,int32_ta* p32tick,int32_ta ms) {
	  int cnt = ms/(s32TimeIntervalSet);
    if(cnt == 0) {
        cnt = 1;
    }
    *pTimer = (*p32tick) + cnt;
}

void  voStartTimerMs(int32_ta* pTimer,int32_ta* p32tick,bool_ta* pstate, int32_ta ms) {
	voSetTimeoutTarget(pTimer,p32tick,ms);
	*pstate = true;
}


bool_ta boIsTargetTimout(int32_ta* pTimer,int32_ta* p32tick) {
		return ( (*p32tick) > (*pTimer));
}


void voStopTimer(int32_ta* pTimer,int32_ta* p32tick,bool_ta* pstate)
{
	   *pstate  = false;
	   *p32tick = 0;
     *pTimer  = *p32tick+5;
}

