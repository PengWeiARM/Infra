//******************************************************************************
//*
//*           Copyright (c) 2015 xx-xx-xx Corporation
//*                  ALL RIGHTS RESERVED
//*
//******************************************************************************

//******************************************************************************
//*  $Id: commTask.c 1686 2015-03-25 10:14:45Z  $
//*
//*Description:
//*******************************************************************************
#define PACE_GLOBLS
#include "main.h"
#include "typedef.h"
#include "stask.h"
#include "ModbusMaster.h"
#include "string.h"
#include "Protocol.h"
#include "PaceBms.h"

typedef struct
{
    u16 Length;
    u16 Index; 
    u8 Buffer[cModbusMasterTxSizeMax];
}ModbusMasterTx_t; 

static ModbusMasterInit_t *pApp;
static bool RxIsSuspended;
static u16 RxTimeOutCnt;
static u16 TxTimeOutCnt;

typedef struct
{
	u8 ModbusSlaveAddr;
 	u8 ModbuSlaveFunCode;
 	u8 ModbusMasterSendSlaveAddr;
 	u8 ModbusMasterSendFunCode;
	u16 ModbusMasterSendRegAddr;
	u16 ModbusMasterSendRegNum;
}ModbusMasterSendSlaveInfo_t;




ModbusMasterRx_t stModbusMasterRx;
static ModbusMasterTx_t stModbusMasterTx;
static ModbusMasterSendSlaveInfo_t stModbusMasterSendSlaveInfo;




const static uint16_t CrcTable[256] =
{
    0x0000,0xC1C0,0x81C1,0x4001,0x01C3,0xC003,0x8002,0x41C2,0x01C6,0xC006,
    0x8007,0x41C7,0x0005,0xC1C5,0x81C4,0x4004,0x01CC,0xC00C,0x800D,0x41CD,
    0x000F,0xC1CF,0x81CE,0x400E,0x000A,0xC1CA,0x81CB,0x400B,0x01C9,0xC009,
    0x8008,0x41C8,0x01D8,0xC018,0x8019,0x41D9,0x001B,0xC1DB,0x81DA,0x401A,
    0x001E,0xC1DE,0x81DF,0x401F,0x01DD,0xC01D,0x801C,0x41DC,0x0014,0xC1D4,
    0x81D5,0x4015,0x01D7,0xC017,0x8016,0x41D6,0x01D2,0xC012,0x8013,0x41D3,
    0x0011,0xC1D1,0x81D0,0x4010,0x01F0,0xC030,0x8031,0x41F1,0x0033,0xC1F3,
    0x81F2,0x4032,0x0036,0xC1F6,0x81F7,0x4037,0x01F5,0xC035,0x8034,0x41F4,
    0x003C,0xC1FC,0x81FD,0x403D,0x01FF,0xC03F,0x803E,0x41FE,0x01FA,0xC03A,
    0x803B,0x41FB,0x0039,0xC1F9,0x81F8,0x4038,0x0028,0xC1E8,0x81E9,0x4029,
    0x01EB,0xC02B,0x802A,0x41EA,0x01EE,0xC02E,0x802F,0x41EF,0x002D,0xC1ED,
    0x81EC,0x402C,0x01E4,0xC024,0x8025,0x41E5,0x0027,0xC1E7,0x81E6,0x4026,
    0x0022,0xC1E2,0x81E3,0x4023,0x01E1,0xC021,0x8020,0x41E0,0x01A0,0xC060,
    0x8061,0x41A1,0x0063,0xC1A3,0x81A2,0x4062,0x0066,0xC1A6,0x81A7,0x4067,
    0x01A5,0xC065,0x8064,0x41A4,0x006C,0xC1AC,0x81AD,0x406D,0x01AF,0xC06F,
    0x806E,0x41AE,0x01AA,0xC06A,0x806B,0x41AB,0x0069,0xC1A9,0x81A8,0x4068,
    0x0078,0xC1B8,0x81B9,0x4079,0x01BB,0xC07B,0x807A,0x41BA,0x01BE,0xC07E,
    0x807F,0x41BF,0x007D,0xC1BD,0x81BC,0x407C,0x01B4,0xC074,0x8075,0x41B5,
    0x0077,0xC1B7,0x81B6,0x4076,0x0072,0xC1B2,0x81B3,0x4073,0x01B1,0xC071,
    0x8070,0x41B0,0x0050,0xC190,0x8191,0x4051,0x0193,0xC053,0x8052,0x4192,
    0x0196,0xC056,0x8057,0x4197,0x0055,0xC195,0x8194,0x4054,0x019C,0xC05C,
    0x805D,0x419D,0x005F,0xC19F,0x819E,0x405E,0x005A,0xC19A,0x819B,0x405B,
    0x0199,0xC059,0x8058,0x4198,0x0188,0xC048,0x8049,0x4189,0x004B,0xC18B,
    0x818A,0x404A,0x004E,0xC18E,0x818F,0x404F,0x018D,0xC04D,0x804C,0x418C,
    0x0044,0xC184,0x8185,0x4045,0x0187,0xC047,0x8046,0x4186,0x0182,0xC042,
    0x8043,0x4183,0x0041,0xC181,0x8180,0x4040
};

void sModbusMasterClrRxBuffer(void)
{       
    u16 Len = 0;
    for(Len = 0; Len < cModbusMasterRxSizeMax; Len++)
    {
        stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][Len] = 0xFF;
    }
	stModbusMasterRx.Index = 0;
}
    
static void sModbusMasterClrTxBuffer(void)
{
    stModbusMasterTx.Length = 0;
    stModbusMasterTx.Index = 0;
}



void sModbusMasterInit(ModbusMasterInit_t *pInit)
{
    pApp = pInit;
    RxTimeOutCnt = 0;
    TxTimeOutCnt = 0;
    RxIsSuspended = FALSE;
    sModbusMasterClrRxBuffer();
    sModbusMasterClrTxBuffer();
    sModbusMasterEnableSend();

}

static u16 sModbusMasterGetCrc(u8 *pData,u16 Lenghth)
{
    u8 CrcHi = 0xFF; //高CRC字节初始化
    u8 CrcLo = 0xFF; //低CRC字节初始化
    u16 Index = 0;   //CRC循环中的索引

    while(Lenghth--)
    {
        Index = CrcHi ^ *pData++;
        CrcHi = CrcLo ^ (CrcTable[Index] >> 8);
        CrcLo = (CrcTable[Index] & 0xFF);
    }
    return (((u16)CrcLo << 8) | (CrcHi & 0xFF));
}

void sModbusMasterEnableRx()
{
    RxIsSuspended = FALSE;
}

void sModbusMasterDisableRx()
{
    RxIsSuspended = TRUE;
}

void sModbusMasterReceiveData(void)
{ 
	stModbusMasterRx.ParseIndex = stModbusMasterRx.ReceiveIndex;
	
    if(RxIsSuspended) return;
    RxTimeOutCnt = 0;
	stCommCB.commFail = FALSE;
    //判断是否越界
    if (stModbusMasterRx.Index >= cModbusMasterRxSizeMax)
    {
        return;
    }
	//memmove(stModbusMasterRx.Buffer,stModbusMasterRx.Buffer_Temp,stModbusMasterRx.Index);
	/*debug调试时需要读取02地址
	if(stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][0] != sGetSendSlaveAddr())
    {
		if(stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][1] !=sGetSendFunCode())
      stModbusMasterRx.Index = 0;
    }
    */
	if(stModbusMasterRx.ReceiveIndex == FALSE)
		{stModbusMasterRx.ReceiveIndex = TRUE;}
	else
		{stModbusMasterRx.ReceiveIndex = FALSE;}
    //MODBUS数据处理过程中
    if(stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][1] == 0x30)
    	{
    	proRxIndex[id_bms_sci] = stModbusMasterRx.Index;
		 my_memmove(pProRxBuffer[id_bms_sci],stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex],stModbusMasterRx.Index);
	}
    
}

void sModbusMasterEnableSend(void)
{
    pApp->pEnableSendSwich();
}

void sModbusMasterEnableReceive(void)
{
    pApp->pEnableReceiveSwich();
}

void sModbusMasterSendData(void)
{
	
    if(stModbusMasterTx.Index < stModbusMasterTx.Length)
    {
        TxTimeOutCnt = 0;
        sModbusMasterEnableSend();
        pApp->pUartSend(stModbusMasterTx.Buffer[stModbusMasterTx.Index++]);
		
    }
    else
    {
        sModbusMasterEnableReceive();
        sModbusMasterClrTxBuffer();
    }
}

bool sModbusMasterIsSendEnd(void)
{
    return(stModbusMasterTx.Length == 0 ? (bool)TRUE : (bool)FALSE);
}

u8 sGetModbusSlaveAddr(void)
{
    return stModbusMasterSendSlaveInfo.ModbusSlaveAddr;
}

u8 sGetModbusSlaveFunCode(void)
{
    return stModbusMasterSendSlaveInfo.ModbuSlaveFunCode;
}

u8 sGetSendSlaveAddr(void)
{
    return stModbusMasterSendSlaveInfo.ModbusMasterSendSlaveAddr;
}

u8 sGetSendFunCode(void)
{
    return stModbusMasterSendSlaveInfo.ModbusMasterSendFunCode;
}


static void sParse0304ReadReponse(void)
{
	
    u16 Temp_RegAddr = stModbusMasterSendSlaveInfo.ModbusMasterSendRegAddr;
	u16 Temp_Len = stModbusMasterSendSlaveInfo.ModbusMasterSendRegNum;
	u16 Temp_EndAddr = Temp_RegAddr + Temp_Len -1;
	u16 Temp_star_index = sSearchAppReg(Temp_RegAddr,pApp->pModbusReg, pApp->RegNum);
	u16 Temp_End_index = sSearchAppReg(Temp_EndAddr,pApp->pModbusReg, pApp->RegNum);
	u16 Temp_index = 3;
	s16 Temp_Data = 0;
	s16 *pTmp;
	
	for(; Temp_star_index < Temp_End_index; Temp_star_index++)
    {
    pTmp = (s16*)(pApp->pModbusReg[Temp_star_index].pData);
    Temp_Data = (((s16)stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][Temp_index]<<8) + stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][Temp_index+1]);
	*pTmp = Temp_Data;
		Temp_index += 2;
	}
	//stCommCB.commFail = FALSE;
}

void sModbusMasterSendCmd(u8 addr, u8 FunCode, u16 RegAddr, u16 RegLenth, u16 *pData)
{
	
    u16 Crc;
    stModbusMasterTx.Buffer[0] = addr;
    stModbusMasterSendSlaveInfo.ModbusMasterSendSlaveAddr = addr;
    stModbusMasterTx.Buffer[1] = FunCode;
    stModbusMasterSendSlaveInfo.ModbusMasterSendFunCode = FunCode;
    stModbusMasterTx.Buffer[2] = (u8)(RegAddr>>8);
    stModbusMasterTx.Buffer[3] = (u8)(RegAddr&0xff);
	stModbusMasterSendSlaveInfo.ModbusMasterSendRegAddr = RegAddr; //高在前，低在后
	stModbusMasterSendSlaveInfo.ModbusMasterSendRegNum = RegLenth;
    stModbusMasterTx.Index = 4;
    switch (FunCode)
    {
        case MODBUS_M_03_READ:
        case MODBUS_M_04_READ:
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(RegLenth>>8);
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(RegLenth&0xff);
            break;
        case MODBUS_M_SINGLE_WRITE:
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(pData[0]>>8);
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(pData[0]&0xff);
            break;
            
        case MODBUS_M_MULTI_WRITE:
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(RegLenth>>8);
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(RegLenth&0xff);
            stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = 2*RegLenth;
            for(u16 i=0; i<RegLenth; i++)
            {
                stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(pData[i]>>8);
                stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(pData[i]&0xff);
            }
            break;
            
    }
    Crc = sModbusMasterGetCrc(stModbusMasterTx.Buffer,stModbusMasterTx.Index);

    //CRC 低字在前，高字节在后
    stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = Crc & 0xFF;
    stModbusMasterTx.Buffer[stModbusMasterTx.Index++] = (u8)(Crc >> 8);
    stModbusMasterTx.Length = stModbusMasterTx.Index;
    stModbusMasterTx.Index = 0;
    sModbusMasterSendData();
	            stCommCB.commFail = TRUE;
}


ModbusMasterResult_t sModbusMasterParse(void)
{
    u16 ubChksumIdx;
    u16 Crc = 0;
    if (stModbusMasterRx.Index >= cModbusMasterRxSizeMax)
    {
        stModbusMasterRx.Index = 0;
    }

     switch(stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][1])
    {
        case MODBUS_M_03_READ:
        case MODBUS_M_04_READ:
            ubChksumIdx = stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][2] + 3;
            break;
        case MODBUS_M_SINGLE_WRITE:
            ubChksumIdx = 6;
            break;
        case MODBUS_M_MULTI_WRITE:
            ubChksumIdx = 6;
            break;

        case MODBUS_M_03_READ_ERROR:
        case MODBUS_M_04_READ_ERROR:
        case MODBUS_M_SINGLE_WRITE_ERROR:
        case MODBUS_M_MULTI_WRITE_ERROR:
            ubChksumIdx = 3;
            break;
        
        default:
            return eFunCodeError;
    }
    if(ubChksumIdx+1 >= cModbusMasterRxSizeMax)
    	{
		return eBuffOverflow;
    	}

    Crc =((u16)stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][ubChksumIdx+1] << 8) + (stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][ubChksumIdx]);
    
    if(Crc != sModbusMasterGetCrc(stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex],ubChksumIdx))
    {
		sModbusMasterClrRxBuffer();
        return eFrameError;
    }
    else
    {   
        stModbusMasterSendSlaveInfo.ModbuSlaveFunCode = stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][1];
        stModbusMasterSendSlaveInfo.ModbusSlaveAddr = stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][0];
        if(sGetSendSlaveAddr() != sGetModbusSlaveAddr())
        {
            sModbusMasterClrRxBuffer();
            return eAddrError;
        }
		if(sGetSendFunCode() != sGetModbusSlaveFunCode())
		{
			sModbusMasterClrRxBuffer();
            return eFunCodeError;
		}
		if(stModbusMasterSendSlaveInfo.ModbusMasterSendRegNum*2 != stModbusMasterRx.Buffer[stModbusMasterRx.ParseIndex][2])
    	{
    		return eFrameError;
		}
        switch(sGetModbusSlaveFunCode())
        {
            case MODBUS_M_03_READ:
            case MODBUS_M_04_READ:
                sParse0304ReadReponse();
                break;
            case MODBUS_M_SINGLE_WRITE:
                break;
            case MODBUS_M_MULTI_WRITE:
                break;

            case MODBUS_M_03_READ_ERROR:
            case MODBUS_M_04_READ_ERROR:
            case MODBUS_M_SINGLE_WRITE_ERROR:
            case MODBUS_M_MULTI_WRITE_ERROR:
                break;
            
            default:
                return eFrameError;
        }        
        sModbusMasterClrRxBuffer();
        return eResponseOK;
    }
}


void sChkModbusMasterComm(void)
{
    if(RxTimeOutCnt < pApp->TimeOutSet)
    {  
        RxTimeOutCnt++;
    }
    else if(RxTimeOutCnt == pApp->TimeOutSet)
    {
        RxTimeOutCnt++;
        stModbusMasterRx.Index = 0;
    }

    if(TxTimeOutCnt < pApp->TimeOutSet)
    {  
        TxTimeOutCnt++;
    }
    else if(TxTimeOutCnt == pApp->TimeOutSet)
    {
        TxTimeOutCnt++;
        sModbusMasterClrTxBuffer();
        sModbusMasterEnableReceive();
    }

}





////////////////////////////////////////
