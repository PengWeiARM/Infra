#include "Kernel.h"


#define SetEvent(event, id)  (event = (event|EVENT(id)))

#define checkEventID(event, id) ((event) & EVENT(id))
#define checkEventIDExt(event,eventMask)   (((event) & (eventMask))==(eventMask))
#define checkEvent(event,eventMask) ((event) & (eventMask))
#define ClearEvent(event, id) (event = ((event) & ~(EVENT(id))))



typedef struct{
    TASK_EVENT value;
    TASK_EVENT mask;
}taskEvent_t;

typedef struct{
    osThreadId posThreadId;
    osSemaphoreId SemaphoreId;
    taskEvent_t Event;
    u32  TimerPeriod;        /*      timer period            */
    s32  targetTime;
}OS_TCB;


OS_TCB stOSTCB[cMaxTaskNum] = {0};
s32 dwTimeTick = 0;

u16 uwIDIndex = 0;

bool OSCheckEvent(TASK_EVENT Group, u32 event)
{
    if(checkEventID(Group, event))
    {
        return TRUE;
    }
    return FALSE;
}

u16 OSGetTaskNum(void)
{
    return (uxTaskGetNumberOfTasks());
}

u16 OSGetUserTaskNum(void)
{
    return (uwIDIndex);
}


u16 OSGetMaxTaskId(void)
{
    return (uwIDIndex);
}

bool OSAddThread(osThreadId ThreadId, osSemaphoreId SemaphoreId)
{
    if(uwIDIndex < cMaxTaskNum - 1)
    {
        if(uwIDIndex+1 > uxTaskGetNumberOfTasks())
        {
            return FALSE;
        }
        stOSTCB[uwIDIndex].posThreadId = ThreadId;
        stOSTCB[uwIDIndex].SemaphoreId = SemaphoreId;
        stOSTCB[uwIDIndex].TimerPeriod = 0;
        stOSTCB[uwIDIndex].Event.value = 0;
        uwIDIndex++;
        return TRUE;
    }
    return FALSE;
}

TASK_EVENT OSMaskEventPend(osThreadId ThreadId, TASK_EVENT mask)
{
    u16 i;
    TASK_EVENT event; 
    osStatus xReturn = osErrorValue;
    for(i=0; i<uwIDIndex; i++)
    {
        if(ThreadId == stOSTCB[i].posThreadId) break;
    }
    portENTER_CRITICAL();
    stOSTCB[i].Event.mask = mask;
    portEXIT_CRITICAL();
    while(1)
    {   
        xReturn = (osStatus)osSemaphoreWait(stOSTCB[i].SemaphoreId, osWaitForever);
        if(osOK == xReturn) 
        {
            event = stOSTCB[i].Event.value;
            stOSTCB[i].Event.value = 0;
            return event;
        }
    }
}

void OSIncTick(void)
{
    portENTER_CRITICAL();
    dwTimeTick++;
    portEXIT_CRITICAL();
}


s32 knlGetTargetTime(u32 msTime)
{
    s32 tmp;
    tmp = dwTimeTick + msToTicks(msTime);
    if(tmp == 0)tmp = 1;
    return tmp;
}


bool knlChkTimeOut(s32* targetTime)
{
    bool res;
    if(*targetTime == 0)return TRUE;
    res = (dwTimeTick - *targetTime) >= 0 ? (bool)true : (bool)false;
    if(res)*targetTime = 0;
    return res;
}

void OSDelayTask(osThreadId ThreadId, u32 time)
{   
    portENTER_CRITICAL();    
    OS_TCB *p = stOSTCB;
    for(u8 i=0; i<uwIDIndex; i++)
    {
        if(p[i].posThreadId == ThreadId)
        {
            ClearEvent(p[i].Event.value, 0);
            p[i].targetTime = dwTimeTick + msToTicks(time);
            break;
        }
    }
    portEXIT_CRITICAL();
}

void OSTimerStart(osThreadId ThreadId, u32 period)
{
    OS_TCB *p = stOSTCB;
    for(u8 i=0; i<uwIDIndex; i++)
    {
        if(p[i].posThreadId == ThreadId)
        {
            p[i].targetTime = knlGetTargetTime(period);
            p[i].TimerPeriod = period;
            break;
        }
    }
}

void OSTimerStop(osThreadId ThreadId)
{
    OS_TCB *p = stOSTCB;
    for(u8 i=0; i<uwIDIndex; i++)
    {
        if(p[i].posThreadId == ThreadId)
        {
            p[i].TimerPeriod = 0;
            break;
        }
    }
}

u32 OSGetTaskTimer(osThreadId ThreadId)
{
    OS_TCB *p = stOSTCB;
    for(u8 i=0; i<uwIDIndex; i++)
    {
        if(p[i].posThreadId == ThreadId)
        {
            return (p[i].TimerPeriod);
        }
    }
    return 0xffff;
}

void OSEventSend(osThreadId ThreadId, u32 event)
{
    u16 i;
    OS_TCB *p = stOSTCB;
    for(i=0; i<uwIDIndex; i++)
    {
        if(ThreadId == p[i].posThreadId) break;
    }

    if((EVENT(event)&(p[i].Event.mask)))
    {
        SetEvent(p[i].Event.value, event);
        osSemaphoreRelease(p[i].SemaphoreId);
    }
}

void OSSendTimerEvent(void)
{
    OS_TCB *p = stOSTCB;
    for(u8 i=0; i<uwIDIndex; i++)
    {
        if(p[i].TimerPeriod != 0)
        {
            if((dwTimeTick - p[i].targetTime) >= 0)
            {
                p[i].targetTime = dwTimeTick + p[i].TimerPeriod;
                if((EVENT(0)&(p[i].Event.mask)))
                {
                    SetEvent(p[i].Event.value, 0);
                    osSemaphoreRelease(p[i].SemaphoreId);
                }
            }
        }
    }
}














