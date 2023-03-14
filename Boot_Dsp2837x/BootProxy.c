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
#include "Protocol.h"
#include "BinUpdater.h"
//###########################################################################
eProtclType_t  em_ProtcolType;
uint16_ta      u16m_node_id = 0;
uint8_ta       u8m_BoardID = 0x30;

uint32_ta      u32BinTotalSize = 0;
uint8_ta       u8HeaderVersion = 0;
uint8_ta       u8DeviceID = 0;
uint8_ta       u8ordinal = 0;
uint8_ta       u8isEncrypted = 0;
//###########################################################################
void voSendOutCANOpenCmd_Reset();
void voSendOutCANOpenCmd_StayInBOOT(bool_ta isStay);
void voSendOutCANOpenCmd_InitiateUpgrade(uint16_ta nodeId);
void voSendOutCANOpenCmd_ResetBootState(uint16_ta nodeId);
void voSendOutCANOpenCmd_RequestIdentification(uint16_ta nodeId);
void voSendOutCANOpenCmd_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);

bool_ta boSendOutCANOpenCmd_StartWriteIn(uint16_ta len);
void voSendOutCANOpenCmd_DataTranferPut(uint8_ta data);
void voSendOutCANOpenCmd_ResetRxBuf();
void voSendOutCANOpenCmd_XcPut(uint8_ta data);
//###########################################################################
void voSendBySciXc_Reset();
void voSendBySciXc_StayInBOOT(bool_ta isStay);
void voSendBySciXc_InitiateUpgrade(uint16_ta nodeId);
void voSendBySciXc_ResetBootState(uint16_ta nodeId);
void voSendBySciXc_RequestIdentification(uint16_ta nodeId);
void voSendBySciXc_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);

void voSendBySciQ_Reset();
void voSendBySciQ_StayInBOOT(bool_ta isStay);
void voSendBySciQ_InitiateUpgrade(uint16_ta nodeId);
void voSendBySciQ_ResetBootState(uint16_ta nodeId);
void voSendBySciQ_RequestIdentification(uint16_ta nodeId);
void voSendBySciQ_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd);


void voSetProtclTypeToUsed(eProtclType_t type);
bool_ta boDataTransferXCmd_StartWriteIn(uint8_ta uid,uint16_ta len);
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
void voSendOutCANOpenCmd_Reset() { 
	
	if( eProtclType_Xc == em_ProtcolType) {
		voSendBySciXc_Reset();
	} else {
		voSendBySciQ_Reset();
	}
} 

void voSendOutCANOpenCmd_StayInBOOT(bool_ta isStay) { 
  if( eProtclType_Xc == em_ProtcolType) {
		voSendBySciXc_StayInBOOT(isStay);
	} else {
		voSendBySciQ_StayInBOOT(isStay);
	}
} 

void voSendOutCANOpenCmd_InitiateUpgrade(uint16_ta nodeId) {

  if( eProtclType_Xc == em_ProtcolType) {
		voSendBySciXc_InitiateUpgrade(nodeId);
	} else {
		voSendBySciXc_InitiateUpgrade(nodeId);
	}	
} 

void voSendOutCANOpenCmd_ResetBootState(uint16_ta nodeId) {
		if( eProtclType_Xc == em_ProtcolType) {
		  voSendBySciXc_ResetBootState(nodeId);
	} else {
		  voSendBySciXc_ResetBootState(nodeId);
	}
}

void voSendOutCANOpenCmd_RequestIdentification(uint16_ta nodeId) { 
	
	if( eProtclType_Xc == em_ProtcolType) {
		voSendBySciXc_RequestIdentification(nodeId);
	} else {
		voSendBySciXc_RequestIdentification(nodeId);
	}
} 

void voSendOutCANOpenCmd_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd) { 
	
	if( eProtclType_Xc == em_ProtcolType) {
		voSendBySciXc_DataTranfer_XXL(nodeId,SeqNo,isEnd);
	} else {
		voSendBySciXc_DataTranfer_XXL(nodeId,SeqNo,isEnd);
	}
} 

bool_ta boSendOutCANOpenCmd_StartWriteIn(uint16_ta len) 
{
	if( eProtclType_Xc == em_ProtcolType) {
		return boDataTransferXCmd_StartWriteIn(id_inner_sci,len);
	} else {
		return false;
	}
}
void voSendOutCANOpenCmd_DataTranferPut(uint8_ta data)
{
	if( eProtclType_Xc == em_ProtcolType) {
		voSendOutCANOpenCmd_XcPut(data);
	} else {}
}

void voSendOutCANOpenCmd_ResetRxBuf()
{
	if( eProtclType_Xc == em_ProtcolType) {

	} else {

	}
}


bool_ta boDataTransferXCmd_StartWriteIn(uint8_ta uid,uint16_ta len)
{
		sResetTxBufIndex( uid);
		sResetTxBufData_BySet(uid ,len);
    return true;
}

void voSendOutCANOpenCmd_XcPut(uint8_ta data) {
	    uint16_ta indexforTx = 0;

    if(proTxInfo[id_inner_sci].ubLen < PRO_MAX_TX_BUF_SIZE_INNER) {
        indexforTx = 19 + proTxInfo[id_inner_sci].ubLen++;   // 7 :Qhead + 10 canoverHead + 1 cmd +1 seq
        pProTxBuffer[id_inner_sci][indexforTx] = data;
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
    uint16_ta lenFor16Bit =0;
    uint8_ta  *pDestAddr=0;
    uint8_ta  *pSrcAddr=0;
    uint16_ta u16CANId_Temp = 0;
		uint8_ta  u8TargetChipAddr =0;
		uint8_ta  u8SrcChipAddr =0;
	
	
    uint8_ta BoardResetMsg[8] = { BOOTMSG_REQUEST_BOARDS_RESET/*0x03*/,0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */

    stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
    stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_BOARDS_RESET;
    stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;
    u16CANId_Temp = CAN_ID_PDO4_REQUEST | CAN_ID_BROADCAST; //to swap H-L.
    stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;// ((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
    stUpgraderCANOpen.uwCANDataLength = 0;

    len       = 10;   //BoardResetMsg(CANOpenData) ? m_comData.m_stUpgraderCANOpen ????
    lenStart  = len;
    pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;
    pSrcAddr  = BoardResetMsg;
    nsize     = sizeof(BoardResetMsg)/sizeof(uint8_ta);

    dataLen_ExtendToBeEven = nsize;
    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
        dataLen_ExtendToBeEven = nsize + 1;
    }

    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
        *pDestAddr++ = *pSrcAddr++;
        len++;
    }
    stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
    lenFor16Bit = len/2;

		svoGetTargetChipAddr(&u8TargetChipAddr);
    svoGetSrcChipAddr   (&u8SrcChipAddr);
		
		proSendSetCmdNoResp_16_Boot(
					id_inner_sci,
					u8TargetChipAddr,
					u8SrcChipAddr,
					db2_upgrader   /* 0x10 */,         /* dataItem */
					(uint16_ta *)&stUpgraderCANOpen,  /* pdata    */
		      0,                                /* offset   */
					lenFor16Bit                       /* len in 16 bit */
				);  
} 

void voSendBySciXc_StayInBOOT(bool_ta isStay) { 
	  uint16_ta i = 0; uint16_ta nsize = 0; uint16_ta len = 0;
    uint16_ta lenStart = 0;uint16_ta dataLen_ExtendToBeEven = 0;
    uint16_ta lenFor16Bit =0;
    uint8_ta *pDestAddr=0;
    uint8_ta *pSrcAddr=0;
    uint16_ta  u16CANId_Temp = 0;
		uint8_ta   u8TargetChipAddr =0;
		uint8_ta   u8SrcChipAddr =0;
	
    uint8_ta StayInBootOnMsg[8] = { BOOTMSG_REQUEST_STAY_IN_BOOT/*0x11*/,1,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */
    uint8_ta StayInBootOffMsg[8] = { BOOTMSG_REQUEST_STAY_IN_BOOT,0,0,0,  0,0,0,0 };          /* fix 8 byte for CANOPENoverSci */

    stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
    stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_STAY_IN_BOOT;
    stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

    u16CANId_Temp = CAN_ID_PDO4_REQUEST | CAN_ID_BROADCAST; //to swap H-L.
    stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
    stUpgraderCANOpen.uwCANDataLength = 0;

    len       = 10;
    lenStart  = len;
    pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;
    if(isStay) {   // 1
        pSrcAddr  = StayInBootOnMsg;
        nsize     = sizeof(StayInBootOnMsg)/sizeof(uint8_ta);
    } else {
        pSrcAddr  = StayInBootOffMsg;
        nsize     = sizeof(StayInBootOffMsg)/sizeof(uint8_ta);
    }
    dataLen_ExtendToBeEven = nsize;
    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
        dataLen_ExtendToBeEven = nsize + 1;
    }

    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
        *pDestAddr++ = *pSrcAddr++;
        len++;
    }
    stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
    lenFor16Bit = len/2;
		
		svoGetTargetChipAddr(&u8TargetChipAddr);
		svoGetSrcChipAddr   (&u8SrcChipAddr);
    proSendSetCmdNoResp_16_Boot(
								id_inner_sci,
								u8TargetChipAddr,
								u8SrcChipAddr,
								db2_upgrader /* 0x10 */,         /* dataItem */
								(uint16_ta*)&stUpgraderCANOpen, /* pdata    */
								0,                              /* offset   */
								lenFor16Bit                     /* len in 16 bit */
							);  
	
} 

void voSendBySciXc_InitiateUpgrade(uint16_ta nodeId) { 
	    int i = 0; int nsize = 0; int len = 0;
     int lenStart = 0;int dataLen_ExtendToBeEven = 0;
     int lenFor16Bit =0;
     uint8_ta *pDestAddr=0;
     uint8_ta *pSrcAddr=0;
     uint16_ta u16CANId_Temp = 0;
		 uint8_ta   u8TargetChipAddr =0;
		 uint8_ta   u8SrcChipAddr =0;

     uint8_ta InitialUpgradeMsg[8] = { BOOTMSG_REQUEST_INITIATE_UPGRADE,0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */
     uint8_ta WindowSize = 255;

     stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
     stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_INITIATE_UPGRADE;
     stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

     u16CANId_Temp = CAN_ID_PDO4_REQUEST | nodeId;           //to swap H-L.
     stUpgraderCANOpen.uwCANID_16 =u16CANId_Temp;            // ((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
     //stUpgraderCANOpen.uwCANID_16 = CAN_ID_PDO4_REQUEST | nodeId;

     stUpgraderCANOpen.uwCANDataLength = 0;
		 
		 voGetBinFileSizeInByte(&u32BinTotalSize);
     u8m_BoardID = u16GetBoardId();   //TranferMachineOridinalToBoardID(m_pUpdter_BinFileInfo->machine,m_pUpdter_BinFileInfo->ordinal);
     u8DeviceID  = u16GetDeviceId();
		 u8ordinal   = u16GetOrdinityID();
		 u8isEncrypted = u16GetEncryptFlag();
		 
     len       = 10;
     lenStart  = len;
     pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;
     if(1) {   /// 1
         InitialUpgradeMsg[1] = WindowSize;
         InitialUpgradeMsg[2] = ((u32BinTotalSize  >> 24) & 0xFF );
         InitialUpgradeMsg[3] = ((u32BinTotalSize  >> 16) & 0xFF );
         InitialUpgradeMsg[4] = ((u32BinTotalSize  >> 8) & 0xFF );
         InitialUpgradeMsg[5] = ((u32BinTotalSize  >> 0) & 0xFF );
         if(u8HeaderVersion >= 5) {
            InitialUpgradeMsg[6] = u8m_BoardID;
         } else {
            InitialUpgradeMsg[6] = u8DeviceID;
         }
         InitialUpgradeMsg[7] = ( (u8ordinal & 0xF) | ((u8isEncrypted & 0xF) << 4) );

         pSrcAddr  = InitialUpgradeMsg;
         nsize     = sizeof(InitialUpgradeMsg)/sizeof(uint8_ta);
     }
     dataLen_ExtendToBeEven = nsize;
     if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
         dataLen_ExtendToBeEven = nsize + 1;
     }

     for(i = 0; i< dataLen_ExtendToBeEven;i++) {
         *pDestAddr++ = *pSrcAddr++;
         len++;
     }
     stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
     lenFor16Bit = len/2;
		 
		 svoGetTargetChipAddr(&u8TargetChipAddr);
		 svoGetSrcChipAddr   (&u8SrcChipAddr);
     proSendSetCmdNoResp_16_Boot(
									id_inner_sci,
									u8TargetChipAddr,
									u8SrcChipAddr,
									db2_upgrader /* 0x10 */,         /* dataItem */
									(uint16_ta*)&stUpgraderCANOpen, /* pdata    */
									0,                              /* offset   */
									lenFor16Bit                     /* len in 16 bit */
								);
} 

void voSendBySciXc_ResetBootState(uint16_ta nodeId) {
		    int i = 0; int nsize = 0; int len = 0;
    int lenStart = 0;int dataLen_ExtendToBeEven = 0;
    int lenFor16Bit =0;
    uint8_ta *pDestAddr=0;
    uint8_ta *pSrcAddr=0;
    uint16_ta  u16CANId_Temp = 0;
		uint8_ta   u8TargetChipAddr =0;
		uint8_ta   u8SrcChipAddr =0;
	
    uint8_ta GetResetBootStateMsg[8] = { BOOTMSG_REQUEST_RESET_BOOTSTATE/*0x01*/,0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */

    stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
    stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_RESET_BOOTSTATE;
    stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

    u16CANId_Temp = CAN_ID_PDO4_REQUEST | nodeId;     // to swap H-L.
    stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;     //((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
  //stUpgraderCANOpen.uwCANID_16 = CAN_ID_PDO4_REQUEST | nodeId;

    stUpgraderCANOpen.uwCANDataLength = 0;

    len       = 10;
    lenStart  = len;
    pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;
    if(1) {   /// 1
        pSrcAddr  = GetResetBootStateMsg;
        nsize     = sizeof(GetResetBootStateMsg)/sizeof(uint8_ta);
    }
    dataLen_ExtendToBeEven = nsize;
    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
        dataLen_ExtendToBeEven = nsize + 1;
    }

    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
        *pDestAddr++ = *pSrcAddr++;
        len++;
    }
    stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
    lenFor16Bit = len/2;
    stUpgraderCANOpen.uwCANDataLength = len - lenStart;
		
		 svoGetTargetChipAddr(&u8TargetChipAddr);
		 svoGetSrcChipAddr   (&u8SrcChipAddr);
     proSendSetCmdNoResp_16_Boot(
										id_inner_sci,
										u8TargetChipAddr,
										u8SrcChipAddr,
										db2_upgrader /* 0x10 */,         /* dataItem */
										(uint16_ta*)&stUpgraderCANOpen, /* pdata    */
										0,                              /* offset   */
										lenFor16Bit                     /* len in 16 bit */
									);
}


void voSendBySciXc_RequestIdentification(uint16_ta nodeId) { 
	    int i = 0; int nsize = 0; int len = 0;
    int lenStart = 0;int dataLen_ExtendToBeEven = 0;
    int lenFor16Bit =0;
    uint8_ta *pDestAddr=0;
    uint8_ta *pSrcAddr=0;
    uint16_ta  u16CANId_Temp = 0;
		uint8_ta   u8TargetChipAddr =0;
		uint8_ta   u8SrcChipAddr =0;
	
    uint8_ta GetIdentificationMsg[8] = { BOOTMSG_REQUEST_GET_IDENTIFICATION/*0x01*/,0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */

    stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
    stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_GET_IDENTIFICATION;
    stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

    u16CANId_Temp = CAN_ID_PDO4_REQUEST | nodeId;     // to swap H-L.
    stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;     //((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
  //stUpgraderCANOpen.uwCANID_16 = CAN_ID_PDO4_REQUEST | nodeId;

    stUpgraderCANOpen.uwCANDataLength = 0;

    len       = 10;
    lenStart  = len;
    pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;
    if(1) {   /// 1
        pSrcAddr  = GetIdentificationMsg;
        nsize     = sizeof(GetIdentificationMsg)/sizeof(uint8_ta);
    }
    dataLen_ExtendToBeEven = nsize;
    if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
        dataLen_ExtendToBeEven = nsize + 1;
    }

    for(i = 0; i< dataLen_ExtendToBeEven;i++) {
        *pDestAddr++ = *pSrcAddr++;
        len++;
    }
    stUpgraderCANOpen.uwCANDataLength = len - lenStart - (dataLen_ExtendToBeEven - nsize);
    lenFor16Bit = len/2;
    stUpgraderCANOpen.uwCANDataLength = len - lenStart;
		
		 svoGetTargetChipAddr(&u8TargetChipAddr);
		 svoGetSrcChipAddr   (&u8SrcChipAddr);
     proSendSetCmdNoResp_16_Boot(
										id_inner_sci,
										u8TargetChipAddr,
										u8SrcChipAddr,
										db2_upgrader /* 0x10 */,         /* dataItem */
										(uint16_ta*)&stUpgraderCANOpen, /* pdata    */
										0,                              /* offset   */
										lenFor16Bit                     /* len in 16 bit */
									);
} 

void voSendBySciXc_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd) { 
	  int i = 0; int nsize = 0; int len = 0;
    int lenStart = 0;int dataLen_ExtendToBeEven = 0;
    int lenFor16Bit =0;
    uint8_ta *pDestAddr=0;
    uint8_ta *pSrcAddr=0;
    uint16_ta u16CANId_Temp = 0;
		uint8_ta   u8TargetChipAddr =0;
		uint8_ta   u8SrcChipAddr =0;
	
    uint8_ta DataTransferXMsg[8] = { BOOTMSG_REQUEST_DATA_TRANSFER,       0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */
    uint8_ta TransferX_EndMsg[8] = { BOOTMSG_REQUEST_DATA_TRANSFER_FINISH,0,0,0,  0,0,0,0 };  /* fix 8 byte for CANOPENoverSci */

     nodeId = nodeId+0; /* node is set by Binupdate For DSP addr ,so just use member-variable */
     DataTransferXMsg[1] = SeqNo;

     stUpgraderCANOpen.uwTarget   = (0x00<<8) + 0x30;
     if(isEnd) {
         stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_DATA_TRANSFER_FINISH;
     }else {
        stUpgraderCANOpen.uwCommand  = (0x00<<8) + progcmd_OverCanOpen_BOOTMSG_REQUEST_DATA_TRANSFER_PLUS;
     }
     stUpgraderCANOpen.uwCANID_32 = (0x00<<8) + 0x00;

     u16CANId_Temp = CAN_ID_PDO4_REQUEST | nodeId;           //to swap H-L.
     stUpgraderCANOpen.uwCANID_16 = u16CANId_Temp;//((u16CANId_Temp & 0xff)<<8) + ((u16CANId_Temp>>8) & 0xff);
     stUpgraderCANOpen.uwCANDataLength = 0;
     len       = 10;
     lenStart  = len;
     pDestAddr = (uint8_ta *)&stUpgraderCANOpen + len;

     if(!isEnd) {   /// 1
         pSrcAddr  = DataTransferXMsg;
         nsize     = sizeof(DataTransferXMsg)/sizeof(uint8_ta);
     } else {
         pSrcAddr  = TransferX_EndMsg;
         nsize     = sizeof(TransferX_EndMsg)/sizeof(uint8_ta);
     }
     dataLen_ExtendToBeEven = 2;         // ??? CAN msg ? ?2?byte  <0x22 seq>
     if( dataLen_ExtendToBeEven % 2 ) {  /* odd for nsize */
         dataLen_ExtendToBeEven = nsize + 1;
     }
     for(i = 0; i< dataLen_ExtendToBeEven;i++) {
         *pDestAddr++ = *pSrcAddr++;
         len++;
     }
		 
		 svoGetTargetChipAddr(&u8TargetChipAddr);
		 svoGetSrcChipAddr   (&u8SrcChipAddr);
     if(!isEnd) {
					stUpgraderCANOpen.uwCANDataLength = 962;  // ->cmd + seq + bin file = 1+1+960
					lenFor16Bit = (len + stUpgraderCANOpen.uwCANDataLength - 2)/2;   //  total Sci_Q frame data <12 + 962 - 2)/2
					proSendSetCmdNoResp_16_Boot_DataTransfX16(
							id_inner_sci,
							u8TargetChipAddr,
							u8SrcChipAddr,
							db2_upgrader /* 0x10 */,        /* dataItem */
							(uint16_ta*)&stUpgraderCANOpen, /* pdata    */
							0,                              /* offset   */
							lenFor16Bit                     /* len in 16 bit */
						);
     } 
		 else {
        stUpgraderCANOpen.uwCANDataLength = 1;  ///just cmd
        lenFor16Bit = (len)/2;
        proSendSetCmdNoResp_16_Boot_DataTransfX16(
							id_inner_sci,
							u8TargetChipAddr,
							u8SrcChipAddr,
							db2_upgrader /* 0x10 */,          /* dataItem */
							(uint16_ta*)&stUpgraderCANOpen,  /* pdata    */
							0,                               /* offset   */
							lenFor16Bit                      /* len in 16 bit */
						);
     }
} 

//###########################################################################
//Functions  : voSendBySciQ_Reset
//Discription: Start to downloa
//###########################################################################
void voSendBySciQ_Reset() { 
} 

void voSendBySciQ_StayInBOOT(bool_ta isStay) { 
} 

void voSendBySciQ_InitiateUpgrade(uint16_ta nodeId) { 
} 

void voSendBySciQ_ResetBootState(uint16_ta nodeId) {
}

void voSendBySciQ_RequestIdentification(uint16_ta nodeId) { 
} 

void voSendBySciQ_DataTranfer_XXL(uint16_ta nodeId,uint16_ta SeqNo,bool_ta isEnd) { 
} 












