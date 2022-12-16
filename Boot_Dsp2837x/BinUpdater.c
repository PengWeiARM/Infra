//###########################################################################
// FILE:    BinUpdater.c
// TITLE:   DSP/MCU
// =========================================================================
// DESCRIPTION:
//!  Main function for BOOT cpu 1
//!  CORE API to manipulator the FSM of Upgrading ....DSP 
//###########################################################################
// Include
//###########################################################################
#include "BinUpdater.h"
#include "FwDownloaderSci.h"
#include "BootProxy.h"
#include "ProtocolData.h"
#include "BootBasicApi.h"
//###########################################################################
eUpdateFsm_t           em_UpdateFsm;
unionCANopenMsg_t      uniCANOpenMsg;
//###########################################################################
uint32_ta             u32Recv_CANId;
uint16_ta             u16Recv_CANLen;
uint16_ta             u16updateLeaveBootMsgCnt = 0;
bool_ta               boOneSessionFinishNotifyFlag = false;

uint16_ta 						u16BoardId = 0;
uint16_ta 						u16device  = 0;
uint16_ta 						u16ordinal = 0;
uint16_ta 						u16encryption = 0;
void 									*pu8Data   = 0;

strBinUpdateFuncApi_t  gstrBinUpdateFuncApi;

bool_ta               bom_IsUpgradeSessionResultSuccess = false;
int32_ta							s32BinCntTimer = 1000;

#define mpBinTimer   (&s32BinCntTimer)
//###########################################################################
__pDataReceived       pDataRecFunc = 0;
__DownloadStart       pDownloadStartFunc = 0; 

 void SessionCallback_RequestIdentification(uint16_ta nodeId);
 void SessionCallback_InitiateUpgrade(uint16_ta nodeId);
 void SessionCallback_DataTransferX(uint16_ta SeqNo);
 void SessionCallback_DataTransferX_End();
 void SessionCallback_SessionComplete(uint16_ta result);

 bool_ta SessionCallback_DataTransferStart(uint16_ta len);
 void SessionCallback_DataTransferPut(uint8_ta data);
 void SessionCallback_ToResetRxBuf();
 
void voBinUpdate_SetProtocolType(eProtclType_t type) ;
//###########################################################################
//###########################################################################
//###########################################################################
void svoBinUpdate_Init() 
{
	voBinUpdate_SetProtocolType(eProtclType_Xc);
}
//###########################################################################
//Functions  : voBinUpdate_FSM()
//Discription: Start to downloa
//###########################################################################
void voBinUpdate_SetProtocolType(eProtclType_t type) {
	
		if( (eProtclType_Xc == type) || (eProtclType_Q == type) )
		{
			pDataRecFunc       = voSci_DownloadMsg_DataReceived;
			pDownloadStartFunc = voSci_DownloadStart;

			gstrBinUpdateFuncApi.pFuncRequestIdentification     = SessionCallback_RequestIdentification;
			gstrBinUpdateFuncApi.pFuncInitUpgrade               = SessionCallback_InitiateUpgrade;		
			gstrBinUpdateFuncApi.pFuncDataTranferX              = SessionCallback_DataTransferX;	
			gstrBinUpdateFuncApi.pFuncDataTranferXEnd    				= SessionCallback_DataTransferX_End;

			gstrBinUpdateFuncApi.pFuncSessionComplete           = SessionCallback_SessionComplete;
			gstrBinUpdateFuncApi.pFuncDataTranferStart       		= SessionCallback_DataTransferStart;		
			gstrBinUpdateFuncApi.pFuncDataTranferPut       			= SessionCallback_DataTransferPut;	
			gstrBinUpdateFuncApi.pFuncResetBuf    							= SessionCallback_ToResetRxBuf;		

		} 
		else  {
			pDataRecFunc = 0;	
			pDownloadStartFunc = 0;

			gstrBinUpdateFuncApi.pFuncRequestIdentification     = 0;
			gstrBinUpdateFuncApi.pFuncInitUpgrade               = 0;		
			gstrBinUpdateFuncApi.pFuncDataTranferX              = 0;	
			gstrBinUpdateFuncApi.pFuncDataTranferXEnd    				= 0;

			gstrBinUpdateFuncApi.pFuncSessionComplete           = 0;
			gstrBinUpdateFuncApi.pFuncDataTranferStart       		= 0;		
			gstrBinUpdateFuncApi.pFuncDataTranferPut       			= 0;	
			gstrBinUpdateFuncApi.pFuncResetBuf    							= 0;		
		}
}

void voBinUpdate_StartSession() {
	  //  if(m_pUpdateSession->isValid()) {
	  if(1) {
			  //m_pProtocolXc->xccleanRecvBuff();
        em_UpdateFsm = eUpdateFSM_SetReset;  /// will goto CMD_Stayinboot in FSM()
		}
}
//#define mpBinTimer   (&s32BinCntTimer)
void voBinUpdate_FSM() {
	
		switch(em_UpdateFsm) {

			case eUpdateFsm_Standby:
				break;
			
			case eUpdateFSM_SetReset:
				if(boIsTargetTimout(mpBinTimer)) {
					voSetTimeoutTarget(mpBinTimer,1*1000);
					voSendOutCANOpenCmd_Reset();
					em_UpdateFsm = eUpdateFsm_StatyInboot;
				}
				break;
			
			case eUpdateFsm_StatyInboot:
				if(boIsTargetTimout(mpBinTimer)) {
					voSetTimeoutTarget(mpBinTimer,1*1000);
					voSendOutCANOpenCmd_StayInBOOT(true);
					em_UpdateFsm = eUpdateFsm_Start;
				}
				break;
			
			case eUpdateFsm_Start:
				if(boIsTargetTimout(mpBinTimer)) {
					voSetTimeoutTarget(mpBinTimer,1*1000);
					if((pDownloadStartFunc) &&(pu8Data) ) {
					  pDownloadStartFunc(u16BoardId,u16device,u16ordinal,u16encryption,pu8Data);
					}
					em_UpdateFsm = eUpdateFsm_Start;
				}
				break;
						
			
			case eUpdateFsm_DataTransfer:
				if(boIsTargetTimout(mpBinTimer)) { }
				break;
			
			case eUpdateFsm_TranferFinsh:
				if(boIsTargetTimout(mpBinTimer)) {
					  if(u16updateLeaveBootMsgCnt++ < 3) {
                voSendOutCANOpenCmd_StayInBOOT(false);
                em_UpdateFsm = eUpdateFsm_TranferFinsh;
            } else {
                em_UpdateFsm = eUpdateFsm_Standby;
                boOneSessionFinishNotifyFlag = true;
            }
				}
				break;
			
			case eUpdateFsm_ErrToCreateSession:
				if(boIsTargetTimout(mpBinTimer)) {
					em_UpdateFsm = eUpdateFsm_Standby;
				}
				break;	
			
			default:
					em_UpdateFsm = eUpdateFsm_Standby;
		}
}

void voBinUpdate_UpdateBinFileInfo() {
	
}

//###########################################################################
//Functions  : voBinUpdate_Handle_SendDataCallback
//Discription: callback
//###########################################################################
//void voBinUpdate_Handle_SendDataCallback(unsigned char* p,uint16_ta len, proDataItem_t item) 
void voBinUpdate_Handle_SendDataCallback(uint8_ta* p,uint16_ta len, uint16_ta item) 	
{

	
}
//###########################################################################
//Functions  : voBinUpdate_Handle_RecDataCallback()
//Discription: callback
//###########################################################################
void voBinUpdate_Handle_RecDataCallback(uint8_ta* p,uint16_ta len ,proDataItem_t item) 	
{
//	  upgraderCANOpen_t *pCANOpen = NULL;
	  uint8_ta    CANOpenCmdGet;
    uint8_ta    i = 0;
    uint8_ta*  pDestAddr = (uint8_ta*)uniCANOpenMsg.DataArry;
    uint16_ta  u16CanIDTemp = 0;
	
	
		if(item == db_upgrader) 
		{
//			pCANOpen = (upgraderCANOpen_t *)p;
//			CANOpenCmdGet = (pCANOpen->uwData[0] & 0xff);
			for(i=0; i<8;i++) {
				*pDestAddr = 0;
				*pDestAddr++ = *(p + 10 +i);   ///10 : length for upgrader-Header = target + cmd + canid32 + CAN-dataLen
			}
		}

	 switch (CANOpenCmdGet) {
		 case BOOTMSG_REQUEST_GET_IDENTIFICATION:
				 break;
		 case BOOTMSG_REQUEST_INITIATE_UPGRADE:
				break;	

			case BOOTMSG_REQUEST_DATA_TRANSFER:
				break;

			case BOOTMSG_REQUEST_BOARDS_RESET:
				break;
			
			case BOOTMSG_REQUEST_STAY_IN_BOOT:
				break;	
			
		 default:
			 break;
	  }
	 
		pDestAddr = (uint8_ta*)uniCANOpenMsg.DataArry;

		//u16CanIDTemp =  pCANOpen->uwCANID_16;     ///Need to Swap !!!
		u32Recv_CANId  = u16CanIDTemp;//
		//u16Recv_CANLen = pCANOpen->uwCANDataLength;

		if(pDataRecFunc) {
			pDataRecFunc(u32Recv_CANId,u16Recv_CANLen,pDestAddr);
		}
	
}

//###########################################################################
//Functions  : SessionCallback_RequestIdentification()
//Discription: callback
//###########################################################################
void SessionCallback_RequestIdentification(uint16_ta nodeId) { 
		voSendOutCANOpenCmd_RequestIdentification(nodeId);
} 

void SessionCallback_InitiateUpgrade(uint16_ta nodeId) { 
	 voSendOutCANOpenCmd_InitiateUpgrade(nodeId);
} 

void SessionCallback_DataTransferX(uint16_ta SeqNo) { 
		voSendOutCANOpenCmd_DataTranfer_XXL(u16m_node_id,SeqNo,false);
} 

void SessionCallback_DataTransferX_End() { 
		voSendOutCANOpenCmd_DataTranfer_XXL(u16m_node_id,0,true);
} 

void SessionCallback_SessionComplete(uint16_ta result) { 
    if(result == 2) { // RESULT_DONE = 2
       bom_IsUpgradeSessionResultSuccess  = true;
    } else {
       bom_IsUpgradeSessionResultSuccess = false;
    }
    em_UpdateFsm = eUpdateFsm_TranferFinsh;
} 


bool_ta SessionCallback_DataTransferStart(uint16_ta len) { 
		voSendOutCANOpenCmd_StartWriteIn(len);
} 

void SessionCallback_DataTransferPut(uint8_ta data) { 
		voSendOutCANOpenCmd_DataTranferPut(data);
} 

void SessionCallback_ToResetRxBuf() { 
	  voSendOutCANOpenCmd_ResetRxBuf();
} 





