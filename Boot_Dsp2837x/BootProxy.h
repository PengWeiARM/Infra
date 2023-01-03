#ifndef  _BOOT_PROXY_H__
#define  _BOOT_PROXY_H__
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
void voSendOutCANOpenCmd_Reset(void);
void voSendOutCANOpenCmd_StayInBOOT(bool_ta isStay);
void voSendOutCANOpenCmd_InitiateUpgrade(uint16_ta nodeId);
void voSendOutCANOpenCmd_RequestIdentification(uint16_ta nodeId);
void voSendOutCANOpenCmd_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);
void voSendOutCANOpenCmd_StartWriteIn(uint16_ta len); 
void voSendOutCANOpenCmd_DataTranferPut(uint8_ta data);
void voSendOutCANOpenCmd_ResetRxBuf(void);
// ##########################################################################################
#ifdef __cplusplus
}
#endif

// ##########################################################################################
#endif // _BOOT_PROXY_H__
