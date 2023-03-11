#ifndef  _BOOT_BASIC_API_H__
#define  _BOOT_BASIC_API_H__
// ##########################################################################################
#include "AppDataType.h"
#include "CANOpenSystem.h"
#include "IFwDownloader.h"
#include "ProtocolType.h"
#include "BootProtocol.h"
#include "Upgrade.h"
// ##########################################################################################
#ifdef __cplusplus
extern "C" {
#endif
// ##########################################################################################


#define mBinUpdateInterval 40  //ms
// ##########################################################################################
void    voTimerTick        (int32_ta* p32tick,bool_ta* pstate);
void    voSetTimeoutTarget (int32_ta* pTimer,int32_ta* p32tick,int32_ta ms);
bool_ta boIsTargetTimout   (int32_ta* pTimer,int32_ta* p32tick);
void    voStopTimer        (int32_ta* pTimer,int32_ta* p32tick,bool_ta* pstate);
void    voStartTimerMs     (int32_ta* pTimer,int32_ta* p32tick,bool_ta* pstate, int32_ta ms);
void voTimeIntervalSet(int32_ta interval);


// ##########################################################################################



// ##########################################################################################
#ifdef __cplusplus
}
#endif

// ##########################################################################################
#endif // _BOOT_BASIC_API_H__
