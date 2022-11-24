#ifndef __KERNEL_H
#define __KERNEL_H

#include "main.h"
#include "cmsis_os.h"
#include "typedef.h"

#define cMaxTaskNum 10

#define	TASK_EVENT			u32
#define BIT(x) (1LL << (x))
#define ANYEVENT (~0)
#define EVENT(id) BIT(id)

#define msToTicks(ms) ((u32)(ms))

#define knlIsTimeOut(time) knlChkTimeOut(&time)

bool OSAddThread(osThreadId ThreadId, osSemaphoreId SemaphoreId);
TASK_EVENT OSMaskEventPend(osThreadId ThreadId, TASK_EVENT mask);
void OSIncTick(void);
s32 knlGetTargetTime(u32 msTime);
bool knlChkTimeOut(s32* targetTime);
void OSDelayTask(osThreadId ThreadId, u32 time);
void OSTimerStart(osThreadId ThreadId, u32 period);
void OSTimerStop(osThreadId ThreadId);
u32 OSGetTaskTimer(osThreadId ThreadId);
void OSEventSend(osThreadId ThreadId, u32 event);
void OSSendTimerEvent(void);
u16 OSGetTaskNum(void);
u16 OSGetMaxTaskId(void);
bool OSCheckEvent(TASK_EVENT Group, u32 event);


#endif




