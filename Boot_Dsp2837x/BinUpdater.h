#ifndef _BIN_UPDATE_H__
#define _BIN_UPDATE_H__
// ##########################################################################################
#include "AppDataType.h"
#include "CANOpenSystem.h"
#include "IFwDownloader.h"
#include "ProtocolType.h"
#include "BootProtocol.h"
#include "DataStruct.h"
// ##########################################################################################
#ifdef __cplusplus
extern "C" {
#endif
// ##########################################################################################
#define mDSP_PROGPAGESIZE 32


typedef enum {
    eUpdateFSM_Init,
    eUpdateFSM_SetReset,
    eUpdateFsm_StatyInboot,
    eUpdateFsm_Start,
    eUpdateFsm_DataTransfer,
    eUpdateFsm_TranferFinsh,
    eUpdateFsm_Standby,
    eUpdateFsm_ErrToCreateSession,
}eUpdateFsm_t;


extern unionCANopenMsg_t      uniCANOpenMsg;
// ##########################################################################################
void svoBinUpdate_Init(void); 
void voBinUpdate_FSM(void);
void voBinUpdate_StartSession(void);

void svoGetTargetChipAddr(uint8_ta* pAddr);
void svoGetSrcChipAddr(uint8_ta* pAddr);

uint16_ta u16GetNodeId(void);
uint16_ta u16GetBoardId(void);
uint16_ta u16GetOrdinityID(void);
uint16_ta u16GetEncryptFlag(void);
uint16_ta u16GetDeviceId(void);
bool_ta boIsSessionFinished(void);
void voResetSessionEndFlag(void);
void voGetBinFileSizeInByte(uint32_ta* pu32Size);
eUpdateFsm_t eGetUpdateFsm(void);
void voBinupdateTimerTick(void);
// ##########################################################################################
#ifdef __cplusplus
}
#endif
// ##########################################################################################
#endif // _BIN_UPDATE_H__
