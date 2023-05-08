#ifndef _FW_DOWNLOAD_SCI_H_
#define _FW_DOWNLOAD_SCI_H_
// ##########################################################################################
#include "AppDataType.h"
#include "CANOpenSystem.h"
#include "IFwDownloader.h"
#include "DataStruct.h"
#include "BootProtocol.h"
// ##########################################################################################
#ifdef __cplusplus
extern "C" {
#endif
// ##########################################################################################


void voSci_DownloadMsg_DataReceived(uint32_ta canIdentifier, uint8_ta length, uint8_ta data[]);
void voSci_DownloadStart(uint16_ta u16BoardId, uint16_ta device, uint16_ta ordinal, uint16_ta encryption, void *p);
void voSci_SetAppSizeToLoader(uint32_ta u32size);
void voDownloadTimerTick(void);
void voDownloadMsgRsp_OnTimeout(void);
void voSci_LowInit(void);
// ##########################################################################################
#ifdef __cplusplus
}
#endif
// ##########################################################################################
#endif // _FW_DOWNLOAD_SCI_H_

