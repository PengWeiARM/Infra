#ifndef  _BOOT_BASIC_API_H__
#define  _BOOT_BASIC_API_H__
// ##########################################################################################
#include "AppDataType.h"
#include "CANOpenSystem.h"
#include "IFwDownloader.h"
#include "ProtocolType.h"
#include "BootProtocol.h"
// ##########################################################################################
#ifdef __cplusplus
extern "C" {
#endif
// ##########################################################################################



// ##########################################################################################
void    voTimeoutByStep();
void    voSetTimeoutTarget(int32_ta* pTimer,int32_ta ms);
bool_ta boIsTargetTimout(int32_ta* pTimer);
void    voStopTimer(int32_ta* pTimer);
void     voStartTimerMs(int32_ta* pTimer,int32_ta ms);

// ##########################################################################################



// ##########################################################################################
#ifdef __cplusplus
}
#endif

// ##########################################################################################
#endif // _BOOT_BASIC_API_H__
