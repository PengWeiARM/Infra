//###########################################################################
// FILE:    FwDownloaderSci.c
// TITLE:   DSP/MCU
// =========================================================================
// DESCRIPTION:
//!  Detail API for Fw downlod by SCI
//!
//###########################################################################
// Include
//###########################################################################
#include "FwDownloaderSci.h"


//###########################################################################
eDownloadState_t  eDownloadSci_State = STATE_IDLE;
uint32_ta         u32m_DataTranferTotoal_RetryCnt = 0;
uint16_ta         u16m_InitgradeRedoCnt = 0;

uint16_ta         u8m_WindowSize = 255;
uint8_ta          u8m_SequenceNumber_Sci = 0;
uint8_ta          u8SeqNo_Old = 0;
static const uint32_ta  u32timeoutMaxRetries = 50;
//###########################################################################
uint32_ta         u32m_length = 0;
uint32_ta         u32m_DataTransferEnd = 0;    

uint32_ta         u32m_device     = 0;
uint32_ta         u32m_ordinal    = 0;
uint32_ta         u32m_encryption = 0;

uint32_ta         u32m_timeoutRetryCounter = 0;
uint32_ta         u32m_NumberOfOutOfSyncRetries = 0;
uint32_ta         u32m_UpdatedBinFileSize_ByByte = 0;
uint32_ta         u32m_TransferResponseIterator = 0;

strBinUpdateFuncApi_t*  pstrBinUpdateFuncApi =  mNULL_ta;//&gstrBinUpdateFuncApi;
//###########################################################################
void voSci_RequestIdentification();
void voSci_InitiateUpgrade();
void voSci_SessionComplete(eDownloadResult_t result);
void voSci_DataTransferX(uint16_ta SeqNo);
void voSci_DataTransferX_End();

bool_ta boSci_DataTransferStart(uint16_ta len);

void voSci_DataTransferPut(uint8_ta data);
void voSci_DownloadMsg_DataReceived(uint32_ta canIdentifier, uint8_ta length, uint8_ta data[]);
void voDownloadMsgRsp_OnTimeout() ;

void voStartTimerMs(int32_ta interval);
void voStopTimer();	
//###########################################################################

//###########################################################################
//Functions  : voSci_DownloadStart
//Discription: Start to downloa
//###########################################################################
void voSci_DownloadStart(uint16_ta u16BoardId, uint16_ta device, uint16_ta ordinal, uint16_ta encryption, void *p) {
		uint32_ta length = 0;
	
	  u32m_length = length;
    u32m_DataTransferEnd = (u32m_length + 959)/960;    

    u32m_device     = device;
    u32m_ordinal    = ordinal;
    u32m_encryption = encryption;

    u32m_timeoutRetryCounter = 0;
    u32m_NumberOfOutOfSyncRetries = 0;
    u32m_UpdatedBinFileSize_ByByte = 0;
	
	  pstrBinUpdateFuncApi = mNULL_ta;//&gstrBinUpdateFuncApi;
	
    voSci_RequestIdentification();  
}
//###########################################################################
//Functions  : GetUpdatedBinFileSizeByByte
//Discription: 
//###########################################################################
uint32_ta GetUpdatedBinFileSizeByByte()
{
    return u32m_UpdatedBinFileSize_ByByte;
}

uint32_ta GetDataRetryCnt()
{
    return u32m_DataTranferTotoal_RetryCnt;
}

//###########################################################################
//Functions  : voSci_DownloadMsg_DataReceived
//Discription: Handle recieved Data
//###########################################################################
void voSci_RequestIdentification() {
	  eDownloadSci_State = STATE_WAITING_FOR_IDENTIFICATION_RESPONSE;
    //StartTimerMs(500);                                               /// 100
    pstrBinUpdateFuncApi->pFuncRequestIdentification(u16m_node_id);
}

void voSci_InitiateUpgrade(){
	 eDownloadSci_State = STATE_WAITING_FOR_INITUPDATE_RESPONSE;
   //StartTimerMs(1*1000);
   pstrBinUpdateFuncApi->pFuncInitUpgrade(u16m_node_id);
}

void voSci_SessionComplete(eDownloadResult_t result){
	
	    if(eDownloadSci_State != STATE_IDLE) {
        eDownloadSci_State = STATE_IDLE;
        pstrBinUpdateFuncApi->pFuncSessionComplete((int)result);
    }
}

void voSci_DataTransferX(uint16_ta SeqNo) {
     pstrBinUpdateFuncApi->pFuncDataTranferX(SeqNo);
}

void voSci_DataTransferX_End() {
	  eDownloadSci_State = STATE_WAITING_FOR_DATATRANSFER_FINISH_RESPONSE;
    pstrBinUpdateFuncApi->pFuncDataTranferXEnd();
}

bool_ta boSci_DataTransferStart(uint16_ta len) {
	  return  pstrBinUpdateFuncApi->pFuncDataTranferStart(len);
}

void voSci_DataTransferPut(uint8_ta data) {
	  pstrBinUpdateFuncApi->pFuncDataTranferPut(data);
}

void voStartTimerMs(int32_ta interval)
{
    //interval = interval;
    //SetTimeoutTarget(interval);
}

void voStopTimer()
{
     //m_TimeoutTarget = m_TimeoutCnt;
}
//###########################################################################
// Functions  : voParseData
// Discription: to parse the data recieved
//###########################################################################
void voParseData() {
}

void voSci_ToDoDataTranferX(eDownloadResult_t result) {
	
	
}
//###########################################################################
// Functions  : voSci_DownloadMsg_DataReceived
// Discription: Handle recieved Data
//###########################################################################
void voSci_DownloadMsg_DataReceived(uint32_ta canIdentifier, uint8_ta length, uint8_ta data[]) 
{
    eDownloadResult_t  result = RESULT_CONTINUE;
	   uint8_ta u8WindowSize    = 0;
	   uint8_ta u8Data           = 0;
	
			//u16m_node_id = u16GetNodeId();
    if ( (canIdentifier & CAN_NODE_ID_MASK) != u16m_node_id )     {
        result = RESULT_UNEXPECTED_RESPONSE;
    }

    if (RESULT_CONTINUE == result) {
        switch (eDownloadSci_State)  {
					case STATE_IDLE: 
						result = RESULT_UNEXPECTED_RESPONSE;
						break;

					case STATE_WAITING_FOR_IDENTIFICATION_RESPONSE: 
						voStopTimer();
						u16m_InitgradeRedoCnt = 0;
						result = RESULT_START_UPGRADE;
						break;

					case STATE_WAITING_FOR_INITUPDATE_RESPONSE: 
						voStopTimer();
						voParseData();
					  u8Data = 0;  // 1st is cmd
					  if(u8Data != BOOTMSG_REQUEST_INITIATE_UPGRADE ) {
							if(u16m_InitgradeRedoCnt++ > 5) {
								result = RESULT_UNEXPECTED_RESPONSE;
								break;
							}
						}
						u8Data = 0;  // 2nd is window_size
						if(u8Data ==0 ) {
								result = RESULT_UPGRADE_RESPONSE_INVALID_WINDOW_SIZE;
								break;
						}
						u8m_WindowSize = u8Data;
						u8WindowSize   = u8m_WindowSize;
						eDownloadSci_State = STATE_WAITING_FOR_DATATRANSFER_RESPONSE;
						
						// the third byte of the message contains the status
						u8Data = 0; //ss.get( c );
						switch (u8Data)  {
								case BOOTMSG_UPGRADE_STAT_OK:             { } break;   // nothing, it is ok
								case BOOTMSG_UPGRADE_STAT_TOO_BIG:        { result = RESULT_UPGRADE_RESPONSE_SIZE_TOO_BIG;    } break;
								case BOOTMSG_UPGRADE_STAT_INVALID:        { result = RESULT_UPGRADE_RESPONSE_INVALID_MESSAGE; } break;
								case BOOTMSG_UPGRADE_WRONG_DEVICE:        { result = RESULT_UPGRADE_WRONG_DEVICE;             } break;
								case BOOTMSG_UPGRADE_UNSUPPORTED_ORDINAL: { result = RESULT_UPGRADE_UNSUPPORTED_ORDINAL;      } break;
								case BOOTMSG_UPGRADE_ERASE_FAILED:        { result = RESULT_UPGRADE_ERASE_FAILED;             } break;
								default:                                  { result = RESULT_UPGRADE_RESPONSE_INVALID_STATUS;  } break;
						}
						voSci_ToDoDataTranferX(result);
						break;

					case STATE_WAITING_FOR_DATATRANSFER_RESPONSE:  
						voStopTimer();
						voParseData();
						u8Data = 0; //ss.get( c );
						if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER )  {
							result = RESULT_UNEXPECTED_RESPONSE;
							if(u8Data == BOOTMSG_REQUEST_DATA_TRANSFER_FINISH) {
								voSci_DataTransferX_End();
								result = RESULT_DONE;
							} else { // if(u8Data == BOOTMSG_REQUEST_DATA_TRANSFER_FINISH) 
								u32m_NumberOfOutOfSyncRetries++;
								if (u32m_NumberOfOutOfSyncRetries < 20)   {  //MAX_OUT_OF_SYNC_RETRIES
									result =  RESULT_CONTINUE;
								}else {
									result = RESULT_MAX_OUT_OF_SYNC_RETRIES_EXCEEDED;
								}
							}
						} else {   // 1. if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER ) 
								u8Data = 0; //ss.get( c );	
								if( (u8Data+1) == u8m_SequenceNumber_Sci )  {
									u32m_TransferResponseIterator++;
									u32m_NumberOfOutOfSyncRetries = 0;
									voSci_ToDoDataTranferX(result);
							}
							else {
									u32m_NumberOfOutOfSyncRetries++;
									if (u32m_NumberOfOutOfSyncRetries < 20)   {  //MAX_OUT_OF_SYNC_RETRIES
											result =  RESULT_CONTINUE;
									}else {
											result = RESULT_MAX_OUT_OF_SYNC_RETRIES_EXCEEDED;
									}
							}
						}    //    2. if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER )  {
						//---------------------------------------------------------------------------------
						// TranferResponse(....)
						break;

					case STATE_WAITING_FOR_DATATRANSFER_FINISH_RESPONSE: 
							voStopTimer();
							voParseData(); // CanRead( ss, length, data );
							u8Data = 0; //ss.get( c );
							if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER_FINISH )    {
									result = RESULT_UNEXPECTED_RESPONSE;
							}  else  {
									u8Data = 0;  // ss.get( c );
									switch (u8Data)       {
											case BOOTMSG_DATA_TRANSFER_FINISH_SUCCESS:              result = RESULT_DONE; break;
											case BOOTMSG_DATA_TRANSFER_FINISH_IMAGE_ERROR:          result = RESULT_UPGRADE_RESPONSE_IMAGE_ERROR; break;
											case BOOTMSG_DATA_TRANSFER_FINISH_CRC_ERROR:            result = RESULT_UPGRADE_RESPONSE_CRC_ERROR; break;
											case BOOTMSG_DATA_TRANSFER_FINISH_FLASH_WRITE_ERROR:    result = RESULT_UPGRADE_RESPONSE_FLASH_WRITE_ERROR; break;
											default:                                                result = RESULT_UNEXPECTED_RESPONSE; break;
									}
							}
						break;

					default:
						break;
				}
	  } //     if (RESULT_CONTINUE == result) {
		// ======================================================================
		// signal the result to the outside world
    if ((RESULT_CONTINUE  != result) && (RESULT_START_UPGRADE != result))     {
       voSci_SessionComplete( result );  
    }
}

//###########################################################################
//Functions  : voDownloadMsgRsp_OnTimeout
//Discription: Handle recieved Data Timeout
//             Re-Sendout 
//###########################################################################
void voDownloadMsgRsp_OnTimeout() 
{
       uint16_ta  u32timeoutMaxRetries = 50;
       uint8_ta   u8SeqNo_Old;

//    if(! IsTargetTimout()) {
//        return;
//    }
    if(STATE_WAITING_FOR_DATATRANSFER_RESPONSE == eDownloadSci_State) {
       //SetTimeoutTarget(1000);
    }
    else {
       //SetTimeoutTarget(2000);
    }
    switch (eDownloadSci_State)
    {
				case STATE_IDLE:      // nothing
					break;
				
				case STATE_WAITING_FOR_IDENTIFICATION_RESPONSE: 
					  if (u32m_timeoutRetryCounter < u32timeoutMaxRetries)  {
                u32m_timeoutRetryCounter++;
                voSci_RequestIdentification(); 
            }  else   {
                 voSci_SessionComplete( RESULT_UNEXPECTED_RESPONSE);
            }
					break;

				case STATE_WAITING_FOR_INITUPDATE_RESPONSE: 
					if (u32m_timeoutRetryCounter < u32timeoutMaxRetries) {
							u32m_timeoutRetryCounter++;                
							voSci_InitiateUpgrade(); 
					} else {
							voSci_SessionComplete( RESULT_UNEXPECTED_RESPONSE);
				  }
				 break;

				case STATE_WAITING_FOR_DATATRANSFER_RESPONSE:  
					  if (u32m_timeoutRetryCounter < u32timeoutMaxRetries)   {
                u32m_timeoutRetryCounter++; 

                if(u8m_SequenceNumber_Sci==0) {   
                    u8SeqNo_Old = 0xff;
                }else{
                    u8SeqNo_Old = u8m_SequenceNumber_Sci -1;
                }
                if(u32m_timeoutRetryCounter % 3 == 0) {
                    pstrBinUpdateFuncApi->pFuncResetBuf();
                }
                voSci_DataTransferX(u8SeqNo_Old);
            }  else  {
                voSci_SessionComplete( RESULT_UNEXPECTED_RESPONSE);
            }
            u32m_DataTranferTotoal_RetryCnt++;
					break;

				case STATE_WAITING_FOR_DATATRANSFER_FINISH_RESPONSE: 
            voSci_SessionComplete( RESULT_DONE);  
					break;

				default:
					break;
		}
}

