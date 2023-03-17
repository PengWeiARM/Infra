#ifndef UPGRADE_H
#define UPGRADE_H

#include "typedef.h"
#include "main.h"


enum 
{
    cInvalidFw = 0,
    cZonergyFw = 1,
    cPaceBmsFw = 2,
};

enum 
{
    cInvalidChip = 0,
    cMasterDsp = 1,
    cSlaveDsp = 2,
    cDisplayArm = 3,
};

enum 
{
    cInvalidPaceChip = 0,
    cPaceBmsChip = 1,
};

		
enum 
{
    cReqTransferFile = 1, //请求传输文件
    cFileCheck = 2,		  //文件校验
    cFilePropCheck = 3,   //文件属性检查
    cReqRdFileProp = 4,   //请求读取文件属性
    cReqUpgrade = 5,
    cStopUpgrade = 6,
};

enum
{
    cRdBinFileProp = 1,
    cEraceBinFile,
    cBinFileChk,
    cBinFilePropChk,
    cWrBinData,
    cArmEnterBootloader,
    eBurnDsp,
};

enum 
{
    cBinFileCheckSumOK = 0,
    cBinFileCheckSumError,
    cEraceBinFileOkay,
    cEraceBinFileFail,
    cBinFilePropOK,
    cBinFilePropError,
    cBinDataWrOK,
    cBinDataWrError,
    cArmEnterBootOK,
    cShutDownOk,
    cShutDownFail,
};

typedef enum 
{
    cBootLevel=1,
    cAppLevel=2,
}runlevel_t;


typedef enum
{
    cZonFwBurnIdle = 0,
    cZonFwBurnPass,
    cZonFwBurnFail,
    cZonFwErasing,
    cZonFwWriting,
    cZonAppCheck,
    cZonBinHeaderCheck,
    cZonBinAppCheck,
    cZonBinFwMatchCheck,
    cZonEnterBoot,
    cZonFwBootBurnIdle = 30,
    cZonFwBootBurnPass,
    cZonFwBootBurnFail,
    cZonFwBootErasing,
    cZonFwBootWriting,
}UpgradeZonFwState_t;

typedef enum
{
    cZonInvalidErrCode = 0,
    cZonFwMismatch,
    cZonFwCheckFail,
    cZonFwEraseFail,
    cZonFwWriteFail,
    cZonFwFwSizeError,
    cZonAppCheckFail,
    cZonEnterAppFail,
    cZonSetOkFlagFail,
    cZonEnterBootFail,
    cZonEraseCmdNack,
    cZonFwBootEraseFail = 30,
    cZonFwBootWriteFail,
    
    
}UpgradeZonFwErrorCode_t;





typedef struct
{
    u16 major;
    u16 minor;    
}ver_t;
typedef struct
{
    ver_t Ver;
    u16 Branch;
    u16 Tag;
}FWVer_t;

typedef struct
{
    u16 model;
    u16 FirmwareType;
    u16 chip;    
    FWVer_t FirmwareVer;    //版本
}BinFile_t;

typedef struct
{
    BinFile_t BinFile;
    u16  RunLevel;  //运行层级 1：app层        2：boot层
    u16  state;  //
    u16  progress;   //升级完成度，单位：%1    
    u16  ErrorCode;  
}remoteUpgrade_t;

typedef struct
{
	u32s_t total;  //BIN文件的字总数
    u16 model;  //机型
    u16 FirmWareType;  //固件类型
	u16 chip;  
    u32s_t addr;  //字偏移地址，从0开始
    u16 Len;  //Data[]的有效长度，单位是字
    u16 Data[512];
}upgradeData_t;

typedef struct
{
    u8 FwType;
    u8 chipNum;
    u8 chip[8];
}FwChip_t;


typedef struct
{
    u8 model;
    FwChip_t ZonergyFwChip;
    FwChip_t PaceBms;    
}ownUpgradeInfor_t;

typedef struct
{
    ownUpgradeInfor_t ownInfor;
    u32 BinFileStartAddr;
    u32 BinFileEndAddr;
    u32 FlashSectorSize;
    u32 AppAreaSize;
    u32 BootVerAddr;
    u32 BootStartAddr;
    u32 BootDataStartAddr;
    u32 BootDataSize;
    void (*pFedDog)(void);
    void (*pReadByteData)(u32 addr, u16 len, u8* data);
    void (*pWriteWordDataWithCheck)(u32 addr, u16 len, u16* data);
    bool (*pVerifyWordData)(u32 addr, u16 len, u16* data);
    void (*pEraseDataArea)(u32 addr);
}UpgradeInit_t;



extern remoteUpgrade_t stRemoteUpgrade;
extern upgradeData_t  stUpgradeData;

void sInitUpgrade(UpgradeInit_t *pInit);
void sRdFileProp(void);
u16 sGetModelInBin(void);
u16 sGetFwTypeInBin(void);
u16 sGetChipInBin(void);
FWVer_t *sGetFwInBin(void);
void sRdBinAppSize(void);
void sRdBinAppCrc(void);


u32 sGetAppSize(void);
u32 sGetAppCrc(void);
u32 sGetAppAreaSize(void);
void sSetUpgradeAction(u8 Type);
u8 sGetUpgradeAction(void);
void sSetUpgradeActionResult(u8 result);
u8 sGetUpgradeActionResult(void);
bool sEraseFileOkay(void);
void sSaveFileProp(u16 *pProp);
bool sWrFileDataIsOK(void);
bool sHeaderCheckIsOk(void);
bool sAppCheckIsOk(void);
bool sFileCheckIsOk(void);
bool sFilePropCheckIsOk(void);
void sMarkTransferFile(s32 time);
bool sIsTransferringFile(void);
bool sIsRdBinFileAppDataOk(u32 addr, u32 len, u8 **pBuff);
bool sIsRdBinFileHeadDataOk(u32 addr, u32 len, u8 **pBuff);
bool sIsRdBinFileDataOk(u32 addr, u32 len, u8 **pBuff);
bool IsReqUpgrade(void);
void sSetReqUpgrade(void);
void sResetReqUpgrade(void);
void sInitRemoteBurn(runlevel_t runLevel);
void sSetRemoteRunLevel(u16 Level);
void sSetRemoteBurnState(u16 state);
void sSetRemoteBurnProgress(u16 progress);
void sSetRemoteBurnErrorCode(u16 errorCode);
void sRestart(void);
void sEnterBootloader(void);
void sSetUserUpgarde(void);
void sResetUserUpgrade(void);
bool sIsUserUpgardeSet(void);
u32 sGetAppStartAddr(void);
u32 sGetAppEndAddr(void);
u32 sGetBootVerAddr(void);
u32 sGetBootStartAddr(void);
u32 sGetBootDataStartAddr(void);
void sGetBootVer(FirmwareVer_t *pVer);
bool sIsEraseAppAreaOk(void);
bool sIsEraseAppPageOk(u32 addr);
bool sIsWrAppAreaOk(u32 addr, u32 len);
bool sIsChipAppCheckOk(void);
void sSetAppOkFlag(void);
void sClrAppOkFlag(void);
bool IsAppOK(void);
bool IsCheckAppOkFlagClrOk(void);
void sClrCheckAppOkFlag(void);
bool sJumptoApplication(u32 AppAddr);
bool sIsNeedCheckAppOk(void);
bool sIsNeedBurnBoot(void);
bool sIsHasBoot(void);
bool sIsBurningBoot(void);
void sBurnBoot(void);



#endif




















