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
#include "BootBasicApi.h"
// #########################################################################
#define mParseBufSize   8
#define mvoParseDATA(x) {voParseData(&x);}
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

int32_ta				     	  s32SciDownloadCntTimer  = 1000;
int32_ta				     	  s32SciDownloadTimerTick = 0;
bool_ta                 boSciDownloadTimerState = 0;

#define mpSciDnldTimer  &s32SciDownloadCntTimer
#define mpSciDnldTick   &s32SciDownloadTimerTick
#define mpSciDnldState   &boSciDownloadTimerState

uint8_ta                u8ParseDataBuf[mParseBufSize+2] = {0};
uint8_ta                u8ParseDatabufIndex = 0;
uint8_ta                u8ArrayBinFile_Test[965]={0x12};
uint8_ta*               pu8BinFileReadout_Start = 0;
uint32_ta               u32m_BinFileByteCnt_readout = 0;


uint32_ta              u32m_DataTransferIterator = 0;


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

void voPrepare_ParseData();
//void voStartTimerMs(int32_ta interval);
//void voStopTimer();	
void voSci_LowInit();

//###########################################################################
//
//
//###########################################################################
void voSci_LowInit() {
		pstrBinUpdateFuncApi    = &gstrBinUpdateFuncApi;
		pu8BinFileReadout_Start = u8ArrayBinFile_Test;
}

//###########################################################################
void voPrepare_ParseData(uint8_ta *p) {
	uint8_ta  i = 0;
	u8ParseDatabufIndex = 0;
	
	for(i=0;i<mParseBufSize;i++) {
		u8ParseDataBuf[i] = *(p++);
	}
}

void  voDownloadTimerTick()
{
	voTimerTick(mpSciDnldTick,mpSciDnldState);
}
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

    u32m_timeoutRetryCounter       = 0;
    u32m_NumberOfOutOfSyncRetries  = 0;
    u32m_UpdatedBinFileSize_ByByte = 0;
	
		u32m_BinFileByteCnt_readout = 0;
		u8m_SequenceNumber_Sci      = 0;
		u32m_DataTransferIterator   = 0;
	
		voStopTimer(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState);
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
    voStartTimerMs(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState,500);                                               /// 100
    pstrBinUpdateFuncApi->pFuncRequestIdentification(u16m_node_id);
}

void voSci_InitiateUpgrade(){
	 eDownloadSci_State = STATE_WAITING_FOR_INITUPDATE_RESPONSE;
   voStartTimerMs(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState,1*1000);   
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

bool_ta boIsBinfileEnd() {
	uint32_ta  u32TotalBinFileSize = 384*1024;   //Dsp-Data + Head-512
	
	if(u32m_UpdatedBinFileSize_ByByte < u32TotalBinFileSize) { // 384K bytes = 384*1024 = 393 216
		return false;	
	} 
  else { 	
		return true; 	
	}
}

bool_ta boIsBinfileHaveData() {
	if(	boIsBinfileEnd()) { return false; } 
  else                  { 
	   return true; 
	}
}
//###########################################################################
// Functions  : voParseData
// Discription: to parse the data recieved
//###########################################################################
bool_ta boTestIshaveData =  false;
uint8_ta u8Temp = 10;
uint8_ta* pu8Temp = &u8Temp;
uint8_ta **g_pBuffTemp = &pu8Temp;


void voParseData(uint8_ta* pdata) {
	*pdata = u8ParseDataBuf[u8ParseDatabufIndex];
	u8ParseDatabufIndex++;
}

void voSci_ToDoDataTranferX(eDownloadResult_t result) {
	  uint16_ta bytes_left = 960;  /////6
	  uint8_ta  u8DataChar = 0;
	  int i = 0;
	  uint16_ta j = 0;
    static uint8_ta  dataFake = 0;
	
	  if(RESULT_CONTINUE == result) {
			if(! pstrBinUpdateFuncApi->pFuncDataTranferStart(960) ) {
				 result = RESULT_READING_FIRMWARE_FILE_FAILED;
			}
		 
			else if (boIsBinfileHaveData()){
				sIsRdBinFileDataOk(u32m_UpdatedBinFileSize_ByByte,960,g_pBuffTemp);
				j = 0;
				dataFake++;
				do {
						 if (boIsBinfileHaveData() )  {  // if (boTestIshaveData) {
								 u8DataChar = *((uint8_ta *)(*g_pBuffTemp) + j); // *pBuff = crcBuff
							   //u8DataChar = dataFake;
							   j++;
								 pstrBinUpdateFuncApi->pFuncDataTranferPut(u8DataChar);
						 }  else   {
								 result = RESULT_READING_FIRMWARE_FILE_FAILED;
						 }					
				 }while((--bytes_left) && (!boIsBinfileEnd())) ;
				 if (RESULT_CONTINUE == result)   {
					pstrBinUpdateFuncApi->pFuncDataTranferX(u8m_SequenceNumber_Sci++);
					u32m_DataTransferIterator++;
					u32m_UpdatedBinFileSize_ByByte += 960;
				}
				i++; 
						 
				if (RESULT_CONTINUE == result) {
					u32m_timeoutRetryCounter = 0;
				//voSetTimeoutTarget(mpSciDnldTimer,mpSciDnldTick,(i) * 25 + 500); //StartTimerMs( (i) * 25 + 500 );
					voStartTimerMs    (mpSciDnldTimer,mpSciDnldTick,mpSciDnldState, 500);
				}
			} 
			else {  // file already sent out
				eDownloadSci_State = STATE_WAITING_FOR_DATATRANSFER_FINISH_RESPONSE;
				u32m_timeoutRetryCounter = 0;
		  //voSetTimeoutTarget(mpSciDnldTimer,mpSciDnldTick,2000);
				voStartTimerMs    (mpSciDnldTimer,mpSciDnldTick,mpSciDnldState,2000);
				result = RESULT_CONTINUE;
			}
	 }
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
		 uint8_ta u8Temp           = 0;
	
	  u16m_node_id = u16GetNodeId();
    if ( (canIdentifier & CAN_NODE_ID_MASK) != u16m_node_id )     {
        result = RESULT_UNEXPECTED_RESPONSE;
    }
    voPrepare_ParseData(data);
    if (RESULT_CONTINUE == result) {
        switch (eDownloadSci_State)  {
					case STATE_IDLE: 
						result = RESULT_UNEXPECTED_RESPONSE;
						break;

					case STATE_WAITING_FOR_IDENTIFICATION_RESPONSE: 
						voStopTimer(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState);
					  voParseData(&u8Data);
					  voSci_InitiateUpgrade();
					  
						sSetRemoteBurnState((uint16_ta)cZonEnterBoot);
						u16m_InitgradeRedoCnt = 0;
						result = RESULT_START_UPGRADE;
						break;

					case STATE_WAITING_FOR_INITUPDATE_RESPONSE: 
						voStopTimer(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState);
						voParseData(&u8Data); // mvoParseDATA(u8Data); // 1st is cmd
					  if(u8Data != BOOTMSG_REQUEST_INITIATE_UPGRADE ) {
							if(u16m_InitgradeRedoCnt++ > 5) {
								result = RESULT_UNEXPECTED_RESPONSE;
								break;
							}
						}
						voParseData(&u8Data); // mvoParseDATA(u8Data);  // 2nd is window_size
						if(u8Data ==0 ) {
								result = RESULT_UPGRADE_RESPONSE_INVALID_WINDOW_SIZE;
								break;
						}
						u8m_WindowSize = u8Data;
						u8WindowSize   = u8m_WindowSize;

						
						// the third byte of the message contains the status
						voParseData(&u8Data); // mvoParseDATA(u8Data); //ss.get( c );
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
						
						sSetRemoteBurnState((uint16_ta)cZonFwErasing);
						eDownloadSci_State = STATE_WAITING_FOR_DATATRANSFER_RESPONSE;
						break;

					case STATE_WAITING_FOR_DATATRANSFER_RESPONSE:  
						voStopTimer(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState);
						voParseData(&u8Data); // mvoParseDATA(u8Data); //ss.get( c );
					
						sSetRemoteBurnState((uint16_ta)cZonFwWriting);
						if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER )  {
							result = RESULT_UNEXPECTED_RESPONSE;
							if(u8Data == BOOTMSG_REQUEST_DATA_TRANSFER_FINISH) {
								voSci_DataTransferX_End();
								sSetRemoteBurnState((uint16_ta)cZonAppCheck);
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
								voParseData(&u8Data); // mvoParseDATA(u8Data); //ss.get( c );	
							 u8Temp = (u8Data+1)&0xff;
								if( u8Temp == u8m_SequenceNumber_Sci )  {
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
							voStopTimer(mpSciDnldTimer,mpSciDnldTick,mpSciDnldState);
							voParseData(&u8Data); // mvoParseDATA(u8Data); // CanRead( ss, length, data );
							if( u8Data != BOOTMSG_REQUEST_DATA_TRANSFER_FINISH )    {
									result = RESULT_UNEXPECTED_RESPONSE;
							}  else  {
									voParseData(&u8Data); //  mvoParseDATA(u8Data);  // ss.get( c );
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

    if(! boIsTargetTimout(mpSciDnldTimer,mpSciDnldTick)) {
        return;
    }
    if(STATE_WAITING_FOR_DATATRANSFER_RESPONSE == eDownloadSci_State) {
       voSetTimeoutTarget(mpSciDnldTimer,mpSciDnldTick,500); // 5000
    }
    else {
       voSetTimeoutTarget(mpSciDnldTimer,mpSciDnldTick,500); // 2000
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


