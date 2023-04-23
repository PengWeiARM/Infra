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
void svoBinUpdate_Init(); 
void voBinUpdate_FSM();
void voBinUpdate_StartSession();

void svoGetTargetChipAddr(uint8_ta* pAddr);
void svoGetSrcChipAddr(uint8_ta* pAddr);

uint16_ta u16GetNodeId();
uint16_ta u16GetBoardId();
uint16_ta u16GetOrdinityID();
uint16_ta u16GetEncryptFlag();
uint16_ta u16GetDeviceId();
bool_ta boIsSessionFinished(void);
void voResetSessionEndFlag(void);
void voGetBinFileSizeInByte(uint32_ta* pu32Size);
eUpdateFsm_t eGetUpdateFsm(void);
// ##########################################################################################
#ifdef __cplusplus
}
#endif
// ##########################################################################################
#endif // _BIN_UPDATE_H__
