//###########################################################################
// FILE:    BootProxy.c
// TITLE:   DSP/MCU
// =========================================================================
// DESCRIPTION:
//!  Main function for BOOT 
//!  Communication Interface
//###########################################################################
// Include
//###########################################################################
#include "BootProxy.h"

//###########################################################################
eProtclType_t  em_ProtcolType;
uint16_ta      u16m_node_id = 0;
//###########################################################################
#if 0	//declaration in _BootProxt_h_
void voSendOutCANOpenCmd_Reset(void);
void voSendOutCANOpenCmd_StayInBOOT(bool_ta isStay);
void voSendOutCANOpenCmd_InitiateUpgrade(uint16_ta nodeId);
void voSendOutCANOpenCmd_RequestIdentification(uint16_ta nodeId);
void voSendOutCANOpenCmd_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);

void voSendOutCANOpenCmd_StartWriteIn(uint16_ta len);
void voSendOutCANOpenCmd_DataTranferPut(uint8_ta data);
void voSendOutCANOpenCmd_ResetRxBuf(void);
#endif
//###########################################################################
void voSendBySciXc_Reset(void);
void voSendBySciXc_StayInBOOT(bool_ta isStay);
void voSendBySciXc_InitiateUpgrade(uint16_ta nodeId);
void voSendBySciXc_RequestIdentification(uint16_ta nodeId);
void voSendBySciXc_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);

void voSendBySciQ_Reset(void);
void voSendBySciQ_StayInBOOT(bool_ta isStay);
void voSendBySciQ_InitiateUpgrade(uint16_ta nodeId);
void voSendBySciQ_RequestIdentification(uint16_ta nodeId);
void voSendBySciQ_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);


void voSetProtclTypeToUsed(eProtclType_t type);

//###########################################################################
//Functions  : voSendOutCANOpenCmd_Reset
//Discription: Start to downloa
//###########################################################################
void voSetProtclTypeToUsed(eProtclType_t type) {
	em_ProtcolType = type;
}
//###########################################################################
//Functions  : voSendOutCANOpenCmd_Reset
//Discription: Start to downloa
//###########################################################################
void voSendOutCANOpenCmd_Reset(void)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
		voSendBySciXc_Reset();
	}
	else
	{
		voSendBySciQ_Reset();
	}
}

void voSendOutCANOpenCmd_StayInBOOT(bool_ta isStay)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
		voSendBySciXc_StayInBOOT(isStay);
	}
	else
	{
		voSendBySciQ_StayInBOOT(isStay);
	}
}

void voSendOutCANOpenCmd_InitiateUpgrade(uint16_ta nodeId)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
		voSendBySciXc_InitiateUpgrade(nodeId);
	}
	else
	{
		voSendBySciXc_InitiateUpgrade(nodeId);
	}
}

void voSendOutCANOpenCmd_RequestIdentification(uint16_ta nodeId)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
		voSendBySciXc_RequestIdentification(nodeId);
	}
	else
	{
		voSendBySciXc_RequestIdentification(nodeId);
	}
}

void voSendOutCANOpenCmd_DataTranfer_XXL(uint16_ta nodeId, uint16_ta SeqNo, bool_ta isEnd)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
		voSendBySciXc_DataTranfer_XXL(nodeId, SeqNo, isEnd);
	}
	else
	{
		voSendBySciXc_DataTranfer_XXL(nodeId, SeqNo, isEnd);
	}
}

void voSendOutCANOpenCmd_StartWriteIn(uint16_ta len)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
	}
	else
	{
	}
}
void voSendOutCANOpenCmd_DataTranferPut(uint8_ta data)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
	}
	else
	{
	}
}
void voSendOutCANOpenCmd_ResetRxBuf(void)
{
	if (eProtclType_Xc == em_ProtcolType)
	{
	}
	else
	{
	}
}

//###########################################################################
//Functions  : voSendOutCANOpenCmd_Reset
//Discription: Start to downloa
//###########################################################################
void voSendBySciXc_Reset() {
	uint16_ta i = 0;
	uint16_ta nsize = 0;
	uint16_ta len = 0;
	uint16_ta lenStart = 0;
	uint16_ta dataLen_ExtendToBeEven = 0;
	uint16_ta lenFor16Bit = 0;
	uint8_ta *pDestAddr = 0;
	uint8_ta *pSrcAddr = 0;
	uint16_ta u16CANId_Temp = 0;

	uint8_ta BoardResetMsg[8] = {BOOTMSG_REQUEST_BOARDS_RESET /*0x03*/, 0, 0, 0, 0, 0, 0, 0}; /* fix 8 byte for CANOPENoverSci */

	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_BOARDS_RESET;
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;
	//    u16CANId_Temp = CAN_ID_PDO4_REQUEST | CAN_ID_BROADCAST; //to swap H-L.
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;// ((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANDataLength = 0;

	//    len       = 10;   //BoardResetMsg(CANOpenData) ? m_comData.m_stUpgraderCANOpen ????
	//    lenStart  = len;
	//    pDestAddr = (uint8_tb *)&m_pProtocolXc->m_comData.m_stUpgraderCANOpen + len;
	//    pSrcAddr  = BoardResetMsg;
	//    nsize     = sizeof(BoardResetMsg)/sizeof(uint8_tb);

	//    dataLen_ExtendToBeEven = nsize;
	//    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
	//        dataLen_ExtendToBeEven = nsize + 1;
	//    }

	//    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
	//        *pDestAddr++ = *pSrcAddr++;
	//        len++;
	//    }
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
	//    lenFor16Bit = len/2;

	//    m_pProtocolXc->xcpbpBootCANOpen_SendSetCmdNoResp_16(
	//                  db_upgrader   /* 0x10 */,                                   /* dataItem */
	//                  (uint16_tb*)&m_pProtocolXc->m_comData.m_stUpgraderCANOpen,  /* pdata    */
	//                  lenFor16Bit                                                 /* len in 16 bit */,
	//                  0           );   
	
}

void voSendBySciXc_StayInBOOT(bool_ta isStay)
{
	uint16_ta i = 0;
	uint16_ta nsize = 0;
	uint16_ta len = 0;
	uint16_ta lenStart = 0;
	uint16_ta dataLen_ExtendToBeEven = 0;
	uint16_ta lenFor16Bit = 0;
	uint8_ta *pDestAddr = 0;
	uint8_ta *pSrcAddr = 0;
	uint16_ta u16CANId_Temp = 0;

	uint8_ta StayInBootOnMsg[8] = {BOOTMSG_REQUEST_STAY_IN_BOOT /*0x11*/, 1, 0, 0, 0, 0, 0, 0}; /* fix 8 byte for CANOPENoverSci */
	uint8_ta StayInBootOffMsg[8] = {BOOTMSG_REQUEST_STAY_IN_BOOT, 0, 0, 0, 0, 0, 0, 0};			/* fix 8 byte for CANOPENoverSci */

	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_STAY_IN_BOOT;
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

	//    u16CANId_Temp = CAN_ID_PDO4_REQUEST | CAN_ID_BROADCAST; //to swap H-L.
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;//((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANDataLength = 0;

	//    len       = 10;
	//    lenStart  = len;
	//    pDestAddr = (uint8_tb *)&m_pProtocolXc->m_comData.m_stUpgraderCANOpen + len;
	//    if(isStay) {   /// 1
	//        pSrcAddr  = StayInBootOnMsg;
	//        nsize     = sizeof(StayInBootOnMsg)/sizeof(uint8_tb);
	//    } else {
	//        pSrcAddr  = StayInBootOffMsg;
	//        nsize     = sizeof(StayInBootOffMsg)/sizeof(uint8_tb);
	//    }
	//    dataLen_ExtendToBeEven = nsize;
	//    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
	//        dataLen_ExtendToBeEven = nsize + 1;
	//    }

	//    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
	//        *pDestAddr++ = *pSrcAddr++;
	//        len++;
	//    }
	//    m_pProtocolXc->m_comData.m_stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
	//    lenFor16Bit = len/2;
	//    m_pProtocolXc->xcpbpBootCANOpen_SendSetCmdNoResp_16(
	//                      db_upgrader /* 0x10 */,                                    /* dataItem */
	//                      (uint16_tb*)&m_pProtocolXc->m_comData.m_stUpgraderCANOpen, /* pdata    */
	//                      lenFor16Bit,                                               /* len      */
	//                      0   );
}

void voSendBySciXc_InitiateUpgrade(uint16_ta nodeId)
{
}

void voSendBySciXc_RequestIdentification(uint16_ta nodeId)
{
}

void voSendBySciXc_DataTranfer_XXL(uint16_ta nodeId, uint16_ta SeqNo, bool_ta isEnd)
{
}

//###########################################################################
//Functions  : voSendBySciQ_Reset
//Discription: Start to downloa
//###########################################################################
void voSendBySciQ_Reset()
{
}

void voSendBySciQ_StayInBOOT(bool_ta isStay)
{
}

void voSendBySciQ_InitiateUpgrade(uint16_ta nodeId)
{
}

void voSendBySciQ_RequestIdentification(uint16_ta nodeId)
{
}

void voSendBySciQ_DataTranfer_XXL(uint16_ta nodeId, uint16_ta SeqNo, bool_ta isEnd)
{
}
