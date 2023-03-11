#ifndef __Data_Struct_H__
#define __Data_Struct_H__

///########################################################################
#include "AppDataType.h"

///########################################################################
#define msizeByU32(x)  (sizeof(x)/sizeof(uint32_ta))
#define SIZEOFARRAY(x) (sizeof((x)) / sizeof((x)[0]))
	
#define cTimerInterValSet   50
///########################################################################
typedef  void (*__func_void_Callback)(void) ;
typedef  void (*__pDataReceived)     (uint32_ta canIdentifier, uint8_ta length, uint8_ta data[]);
typedef  void (*__DownloadStart)     (uint16_ta u16BoardId, uint16_ta device, uint16_ta ordinal, uint16_ta encryption, void *p);
///########################################################################
typedef   void (*__pCallback_ResetBootState)(uint16_ta nodeId);
typedef   void (*__pCallback_RequestIdentification)(uint16_ta nodeId);
typedef   void (*__pCallback_InitiateUpgrade)(uint16_ta nodeId);
typedef   void (*__pCallback_DataTransferX)(uint16_ta SeqNo);
typedef   void (*__pCallback_DataTransferX_End)();
typedef   void (*__pCallback_SessionComplete)(uint16_ta result);

typedef   bool_ta (*__pCallback_DataTransferStart)(uint16_ta len);
typedef   void (*__pCallback_DataTransferPut)(uint8_ta data);
typedef   void (*__pCallback_ToResetRxBuf)();
///########################################################################
typedef enum {
    eDCatalog_cpu_bool,
    eDCatalog_cpu_char,
    eDCatalog_cpu_u8,
    eDCatalog_cpu_s8,
    eDCatalog_cpu_u16,
    eDCatalog_cpu_s16,
    eDCatalog_cpu_u32,
    eDCatalog_cpu_s32,
    eDCatalog_cpu_u64,
    eDCatalog_cpu_s64,
    eDCatalog_cpu_f32,
    eDCatalog_cpu_f64,
    eDCatalog_cpu_stdversion,    /* grid standard data structur : version  1.1.118.2352  4bit 4bit 8bit 16bit */
    eDCatalog_cpu_swversion,     /* grid standard data structur : version  1.0.15.2352   ??*/
    eDCatalog_cpu_IpAddr,        /* ip address of net work 192.168.1.1   8bit.8bit.8bit.8bit*/
}eDCatalog_cpu_t;
///########################################################################

typedef enum {
    eChipType_NULL,
    eChipType_DSP_1,
    eChipType_DSP_2,
    eChipType_DSP_3,
    eChipType_MCU_1,
    eChipType_MCU_2,
    eChipType_MCU_3,
    eChipType_FPGA_1,
    eChipType_FPGA_2,
    eChipType_FPGA_3

} eChipID_t;

typedef enum {
    eFwType_NULL,
    eFwType_APP,
    eFwType_BOOT,
    eFwType_TEST
} eFwType_t;

typedef enum {
    eFwHeaderType_NULL,
    eFwHeaderType_Xc,
    eFwHeaderType_Legacy,
    eFwHeaderType_Rsvd
} eFwHeaderType_t;



typedef enum {
  eChnl_Null,
  eChnl_BySciQ,
  eChnl_ByCanOpen,
  eChnl_ByEthernet
}eUpdateChannel_t;

typedef struct {
    bool_ta  isHeaderValid;
    bool_ta  isEncrypted;
    bool_ta  isBinValid;
    uint32_ta   Headsize;        /// Bytes
    uint32_ta   BinContextSize;  /// Bytes
    uint32_ta   BinContextCrc;   /// Bytes
    uint32_ta   ordinal;         /// Bytes
    uint32_ta   BinTotalSize;
    uint32_ta   HeaderVersion;

    eChipID_t eChipType;
    eFwType_t eFwType;

    uint32_ta   MainVerNo;
    uint32_ta   SlaveVerNo;
    uint32_ta   BranchVerNo;
    uint32_ta   SeqVerNo;

    uint32_ta   DeviceID;
    uint32_ta   machine;
//    QString strDeviceId;
//    QString strVersion;
//    QString strBuildNo;

//    QString strMachine;
//    QString strChipType;
//    QString strFwType;

}stBinFileInfo_t;

///########################################################################


typedef struct {
    uint8_ta  cmd;
    uint8_ta  BoardType;
    uint8_ta  SN[4];
    uint8_ta  CanID;
    uint8_ta  HwID;
}stCANOpenMsg_IDentifcation_t;

typedef struct {
    uint8_ta  cmd;
    uint8_ta  maxWinSize;
    uint32_ta FwSize;
    uint8_ta  BOOTDevice;
    uint8_ta  Encrypt_Ordinal;
}stCANOpenMsg_InitUpgrade_t;

typedef struct {
    uint8_ta  cmd;
    uint8_ta  SeqNo;
    uint8_ta  data[6];
}stCANOpenMsg_DataTransfer_t;

typedef struct {
    uint8_ta  cmd;
    uint8_ta  BootDevice;
    uint8_ta  SN[4];
    uint8_ta  Oridinal;
    uint8_ta  HwID;
}stCANOpenMsg_GetVer_t;


typedef union {
    uint8_ta DataArry[8];
    stCANOpenMsg_IDentifcation_t  stMsgIDentification;
    stCANOpenMsg_InitUpgrade_t    stMsgInitUpgrade;
    stCANOpenMsg_DataTransfer_t   stMsgDataTransfer;
    stCANOpenMsg_GetVer_t         stMsgGetVer;
} unionCANopenMsg_t;

typedef struct {
	__pCallback_ResetBootState         pFuncResetBootState;
	__pCallback_RequestIdentification  pFuncRequestIdentification ;
	__pCallback_InitiateUpgrade        pFuncInitUpgrade;
	__pCallback_DataTransferX          pFuncDataTranferX;
	__pCallback_DataTransferX_End      pFuncDataTranferXEnd;
	__pCallback_SessionComplete        pFuncSessionComplete;

	__pCallback_DataTransferStart      pFuncDataTranferStart;
	__pCallback_DataTransferPut        pFuncDataTranferPut ;
	__pCallback_ToResetRxBuf           pFuncResetBuf ;
	
} strBinUpdateFuncApi_t;

extern uint16_ta              u16m_node_id;
extern strBinUpdateFuncApi_t  gstrBinUpdateFuncApi;
///########################################################################

#endif //// __Data_Struct_H__
