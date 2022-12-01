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



// ##########################################################################################
void svoBinUpdate_Init(); 











// ##########################################################################################
#ifdef __cplusplus
}
#endif
// ##########################################################################################
#endif // _BIN_UPDATE_H__
