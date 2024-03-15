//******************************************************************************
//* Include files
//******************************************************************************
#include "Modbus.h"


//******************************************************************************
//*                 the constant definition(only use in local file)
//******************************************************************************
#define cBroadcastAddr  0
//******************************************************************************
//*                 the global variable define
//******************************************************************************
//extern rtc_t sysTime;
//******************************************************************************
//*                 the local variable define
//******************************************************************************
modbusrx_t stModbusRx;
modbustx_t stModbusTx;

static modbusInit_t *pApp;
static u16 RxTimeOutCnt;
static u16 TxTimeOutCnt;
static bool RxIsSuspended;


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


//******************************************************************************
//* Local function Prototypes
//******************************************************************************

static void sModbusClrRxBuffer(void);
static void sModbusClrTxBuffer(void);

void sModbusInit(modbusInit_t *pInit)
{
    pApp = pInit;
    RxTimeOutCnt = 0;
    TxTimeOutCnt = 0;
    RxIsSuspended = FALSE;
    sModbusClrRxBuffer();
    sModbusClrTxBuffer();
    sModbusEnableReceive();

}

/*******************************************************************************
* Function Name  : sModbusGetCrc
* Description    : get modbus CAR verify code
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static u16 sModbusGetCrc(u8 *pData,u16 Lenghth)
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


u16 sCalCrc(u8 *pData,u16 Lenghth)
{
    return sModbusGetCrc(pData, Lenghth);
}

void sModbusEnableRx()
{
    RxIsSuspended = FALSE;
}

void sModbusDisableRx()
{
    RxIsSuspended = TRUE;
}


/*******************************************************************************
* Function Name  : sModbusReceiveData
* Description    : use modbus to receive the serial data;
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void sModbusReceiveData(u8 Data)
{    
    if(RxIsSuspended) return;
    RxTimeOutCnt = 0;
    //判断是否越界
    if (stModbusRx.Index >= cModRxBusSizeMax)
    {
        return;
    }
    if(stModbusRx.Buffer[0] != pApp->OwnAddr)
    {
        if(stModbusRx.Buffer[0] != cBroadcastAddr)
        {
            stModbusRx.Index = 0;
        }
    }
    //MODBUS数据处理过程中
    stModbusRx.Buffer[stModbusRx.Index] = Data;  
    stModbusRx.Index++;
}

void sModbusEnableSend(void)
{
    pApp->pEnableSendSwich();
}

void sModbusEnableReceive(void)
{
    pApp->pEnableReceiveSwich();
}


/*******************************************************************************
* Function Name  : sModbusSendData
* Description    : use modbus to send the serial data;
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void sModbusSendData(void)
{
    if(stModbusTx.Index < stModbusTx.Length)
    {
        TxTimeOutCnt = 0;
        sModbusEnableSend();
        pApp->pUartSend(stModbusTx.Buffer[stModbusTx.Index++]);
    }
    else
    {
        sModbusClrTxBuffer();
    }
}

bool sModbusIsSendEnd(void)
{
    return(stModbusTx.Length == 0 ? (bool)true : (bool)false);
}



/*******************************************************************************
* Function Name  : sModbusChkSendBusy
* Description    : to check the modbus in send preocess, no idle
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
u8 sModbusChkSendBusy(void)
{
    if(stModbusTx.Length == 0)
    {
        return cModbusIdle;
    }
    else
    {
        return cModbusBusy;
    }
}

s16 sSearchAppReg(u16 addr, const modbusRegItem_t *pModbusReg, u16 num)
{
	s16  left = 0;
	s16 right = num-1;
    s16 mid;
	while (left <= right)
	{
		mid = (right + left) / 2;
		if (pModbusReg[mid].RegAddr > addr)
		{
			right = mid - 1;
		}
		else if (pModbusReg[mid].RegAddr < addr)
		{
			left = mid + 1;
		}
		else
		{
			break;
		}
	}
	if (left > right)
	{
        return -1;
    }
    else
    {
        return mid;
    }
}

rwErrorType_t sCheckModbusRwReg(u8 cmd, u16 addr, u16 len, u8 *pBuff, u16 *pIndex)
{
    u16 i;
    if((addr >= pApp->RegStartAddr) && ((addr + len-1) <= pApp->RegEndAddr))
    {   
        s16 index;
        s16 indexSave;
        index = sSearchAppReg(addr, pApp->pModbusReg, pApp->RegNum);
        if(index == -1) return eInvalidDataAddr;
        indexSave = index;
        switch(cmd)
        {
            case MODBUS_03_READ:
                for(i=0; i<len; index++,addr++,i++)
                {
                    if(pApp->pModbusReg[index].RegAddr == addr)
                    {
                        if(((pApp->pModbusReg[index].permission & P03R_XXW) == 0)
                            &&(pApp->pModbusReg[index].Regtype == eUsed))
                        {
                            return eInvalidDataAddr;
                        }
                    }
                    else
                    {
                        return eInvalidDataAddr;
                    }
                }
            break;
                
            case MODBUS_04_READ:
                for(i=0; i<len; index++,addr++,i++)
                {
                    if(pApp->pModbusReg[index].RegAddr == addr)
                    {
                        if(((pApp->pModbusReg[index].permission & P04R_XXW) == 0)
                            &&(pApp->pModbusReg[index].Regtype == eUsed))
                        {
                            return eInvalidDataAddr;
                        }
                    }
                    else
                    {
                        return eInvalidDataAddr;
                    }
                }

            break;

            case MODBUS_SINGLE_WRITE:
                for(i=0; i<len; index++,addr++,i++)
                {
                    if(pApp->pModbusReg[index].RegAddr != addr)
                    {
                        return eInvalidDataAddr;
                    }
                    else if(pApp->pModbusReg[index].Regtype == eReserved)
                    {
                        continue;
                    }
                    else if((pApp->pModbusReg[index].permission & PXXR_06W) == 0)
                    {
                        return eInvalidDataAddr;
                    }                    
                    else if(pApp->pModbusReg[index].RegDatatype == eSigned)
                    {                   
                        s16 tmp = (s16)(pBuff[0+i*2]<<8) + (pBuff[1+i*2]);
                        if((tmp<pApp->pModbusReg[index].min)||(tmp>pApp->pModbusReg[index].max))
                        {
                            return eInvalidDataValue;
                        }
                    }
                    else if(pApp->pModbusReg[index].RegDatatype == eUnsigned)
                    {
                        u16 tmp = (u16)(pBuff[0+i*2]<<8) + (pBuff[1+i*2]);
                        if((tmp<(u16)(pApp->pModbusReg[index].min))||(tmp>(u16)(pApp->pModbusReg[index].max)))
                        {
                            return eInvalidDataValue;
                        }
                    }                    
                }
            break;
                
            case MODBUS_MULTI_WRITE:
                for(i=0; i<len; index++,addr++,i++)
                {
                    if(pApp->pModbusReg[index].RegAddr != addr)
                    {
                        return eInvalidDataAddr;
                    }
                    else if(pApp->pModbusReg[index].Regtype == eReserved)
                    {
                        continue;
                    }
                    else if((pApp->pModbusReg[index].permission & PXXR_10W) == 0)
                    {
                        return eInvalidDataAddr;
                    }
                    else if(pApp->pModbusReg[index].RegDatatype == eSigned)
                    {
                        s16 tmp = (s16)(pBuff[0+i*2]<<8) + (pBuff[1+i*2]);
                        if((tmp<pApp->pModbusReg[index].min)||(tmp>pApp->pModbusReg[index].max))
                        {
                            return eInvalidDataValue;
                        }
                    }
                    else if(pApp->pModbusReg[index].RegDatatype == eUnsigned)
                    {
                        u16 tmp = (u16)(pBuff[0+i*2]<<8) + (pBuff[1+i*2]);
                        if((tmp<(u16)(pApp->pModbusReg[index].min))||(tmp>(u16)(pApp->pModbusReg[index].max)))
                        {
                            return eInvalidDataValue;
                        }
                    }
                    
                }
            break;
            default:
            break;
        }
        *pIndex = (u16)indexSave;
        return eNoError;
        
    }
    else
    {
        return eInvalidDataAddr;
    }    
    
}


void sSendErrorResponse(u8 error)
{   
    u16 Crc;
    stModbusTx.Buffer[0] = stModbusRx.Buffer[0];
    stModbusTx.Buffer[1] = 0x80 | (stModbusRx.Buffer[1]);
    stModbusTx.Buffer[2] = error;
    Crc = sModbusGetCrc(stModbusTx.Buffer,3);
    stModbusTx.Buffer[3] = Crc & 0xFF;
    stModbusTx.Buffer[4] = Crc >> 8;
    stModbusTx.Length = 5;
    stModbusTx.Index = 0;
    sModbusSendData();    
}

/*******************************************************************************
* Function Name  : sModbusParseReadCmd
* Description    : Parse the read command
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static void sModbusParse0304ReadCmd(u8 cmd)
{
    u16 Len = 0;
    u16 StartAddress = 0;
    u16 Crc = 0;
    u16 Cnt = 0;
    u16 Offset = 0;
    u16 ByteCnt;
    rwErrorType_t result;
    u16 tmp;

    //判断接受地址方位是否正确
    StartAddress = ((u16)stModbusRx.Buffer[2] << 8) + stModbusRx.Buffer[3];
    Len = ((u16)stModbusRx.Buffer[4] << 8) + stModbusRx.Buffer[5];
    ByteCnt = Len*2;
    if(ByteCnt>cModBusByteMax)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_LENTH);
        return;
    }

    result = sCheckModbusRwReg(cmd, StartAddress, Len, stModbusRx.Buffer, &Offset);
    if(result == eInvalidDataAddr)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_ADDR);
        return;
    }
    else if(result == eInvalidDataValue)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_VALUE);
        return;
    }

    stModbusTx.Buffer[0] = stModbusRx.Buffer[0];
    stModbusTx.Buffer[1] = stModbusRx.Buffer[1];
    stModbusTx.Buffer[2] = Len * 2; //定义的都是字型，转为字节长度

    stModbusTx.Index = 3;

    for(Cnt = 0; Cnt < Len; Cnt++)
    {
        u16 *pTmp;
        pTmp = (u16*)(pApp->pModbusReg[Offset+Cnt].pData);
        tmp = *pTmp;
        stModbusTx.Buffer[stModbusTx.Index++] = (u8)(tmp >> 8);
        stModbusTx.Buffer[stModbusTx.Index++] = (u8)(tmp & 0xFF);
    }

    Crc = sModbusGetCrc(stModbusTx.Buffer,stModbusTx.Index);

    //CRC 低字在前，高字节在后
    stModbusTx.Buffer[stModbusTx.Index++] = Crc & 0xFF;
    stModbusTx.Buffer[stModbusTx.Index++] = Crc >> 8;

    //计算总的发送长度
    stModbusTx.Length = stModbusTx.Index;
    //发送BUFFER中的首个数据，其他在中断中发送
    stModbusTx.Index = 0;
    sModbusSendData();
}


/*******************************************************************************
* Function Name  : sModbusParseSingleWriteCmd
* Description    : Parse the write command
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static void sModbusParseSingleWriteCmd(void)
{     
    u16 StartAddress = 0;
    u16 Crc = 0;
    u16 Cnt;
    u16 Offset = 0;
    rwErrorType_t result;
    u16 tmp;

    //判断接受地址是否正确
    StartAddress = ((u16)stModbusRx.Buffer[2] << 8) + stModbusRx.Buffer[3];

    result = sCheckModbusRwReg(MODBUS_SINGLE_WRITE, StartAddress, 1, &stModbusRx.Buffer[4], &Offset);
    if(result == eInvalidDataAddr)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_ADDR);
        return;
    }
    else if(result == eInvalidDataValue)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_VALUE);
        return;
    }

    tmp = ((u16)stModbusRx.Buffer[4] << 8) + stModbusRx.Buffer[5];
    if(pApp->pModbusReg[Offset].RegDatatype == eSigned)
    {
        s16 *pTmp;
        pTmp = (s16*)(pApp->pModbusReg[Offset].pData);
        *pTmp = (s16)tmp;
    }
    else if(pApp->pModbusReg[Offset].RegDatatype == eUnsigned)
    {
        u16 *pTmp;
        pTmp = (u16*)(pApp->pModbusReg[Offset].pData);
        *pTmp = (u16)tmp;
    }
    else if(pApp->pModbusReg[Offset].RegDatatype == eAscII)
    {
        u16 *pTmp;
        pTmp = (u16*)(pApp->pModbusReg[Offset].pData);
        *pTmp = (u16)tmp;
    }

    if(!(*(pApp->pModbusReg[Offset].pfunction))(Offset, 1, &result))
    {
        if(result == eInvalidDataAddr)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_ADDR);
        }
        else if(result == eInvalidDataValue)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_VALUE);
        }
        else if(result == eInvalidLen)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_LENTH);
        }
        else if(result == eNack)
        {
            sSendErrorResponse(MODBUS_WRITE_FAILED);
        }
        else
        {
            sSendErrorResponse(MODBUS_WRITE_FAILED);
        }
        return;
    }

    stModbusTx.Index = 0;

    for(Cnt = 0; Cnt < 6; Cnt++)
    {
        stModbusTx.Buffer[stModbusTx.Index++] = stModbusRx.Buffer[Cnt];
    }

    //计算回复数据的CRC
    Crc = sModbusGetCrc(stModbusTx.Buffer,6);
    stModbusTx.Buffer[stModbusTx.Index++] = Crc & 0xFF;
    stModbusTx.Buffer[stModbusTx.Index++] = Crc >> 8;

    //计算总的发送长度
    stModbusTx.Length = stModbusTx.Index;

    //发送BUFFER中的首个数据，其他在中断中发送
    stModbusTx.Index = 0;
    sModbusSendData();
    
}


/*******************************************************************************
* Function Name  : sModbusParseWriteCmd
* Description    : Parse the write command
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static void sModbusParseMultiWriteCmd(void)
{
    u16 StartAddress = 0;
    u16 Crc = 0;
    u16 Cnt;
    u16 Offset = 0;
    rwErrorType_t result;
    u16 tmp;
    u16 ByteCnt = 0;
    u16 Len;
    
    //判断接受地址是否正确
    StartAddress = ((u16)stModbusRx.Buffer[2] << 8) + stModbusRx.Buffer[3];
    Len = ((u16)stModbusRx.Buffer[4] << 8) + stModbusRx.Buffer[5];

    //发送的字节数，错误
    ByteCnt = stModbusRx.Buffer[6];
	if(ByteCnt != (Len*2))
	{
	    sSendErrorResponse(MODBUS_INVALID_DATA_LENTH);
	    return;
	}
    
    if(ByteCnt>cModBusByteMax)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_LENTH);
        return;
    }

    result = sCheckModbusRwReg(MODBUS_MULTI_WRITE, StartAddress, Len, &stModbusRx.Buffer[7], &Offset);
    if(result == eInvalidDataAddr)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_ADDR);
        return;
    }
    else if(result == eInvalidDataValue)
    {
        sSendErrorResponse(MODBUS_INVALID_DATA_VALUE);
        return;
    }

    for(Cnt = 0; Cnt < Len; Cnt++)
    {   
        if((pApp->pModbusReg[Offset].pfunction) != (pApp->pModbusReg[Offset+Cnt].pfunction))
        {
            sSendErrorResponse(MODBUS_INTERNAL_ERROR);
            return;
        }
    }

    for(Cnt = 0; Cnt < Len; Cnt++)
    {
        tmp = ((u16)stModbusRx.Buffer[7+Cnt*2] << 8) + stModbusRx.Buffer[8+Cnt*2];
        if(pApp->pModbusReg[Offset+Cnt].RegDatatype == eSigned)
        {
            s16 *pTmp;
            pTmp = (s16*)(pApp->pModbusReg[Offset+Cnt].pData);
            *pTmp = (s16)tmp;
        }
        else if(pApp->pModbusReg[Offset+Cnt].RegDatatype == eUnsigned)
        {
            u16 *pTmp;
            pTmp = (u16*)(pApp->pModbusReg[Offset+Cnt].pData);
            *pTmp = (u16)tmp;
        }
        else if(pApp->pModbusReg[Offset+Cnt].RegDatatype == eAscII)
        {
            u16 *pTmp;
            pTmp = (u16*)(pApp->pModbusReg[Offset+Cnt].pData);
            *pTmp = (u16)tmp;
        }
    }

    if(!(*(pApp->pModbusReg[Offset].pfunction))(Offset, Len, &result))
    {
        if(result == eInvalidDataAddr)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_ADDR);
        }
        else if(result == eInvalidDataValue)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_VALUE);
        }
        else if(result == eInvalidLen)
        {
            sSendErrorResponse(MODBUS_INVALID_DATA_LENTH);
        }
        else if(result == eNack)
        {
            sSendErrorResponse(MODBUS_WRITE_FAILED);
        }
        else
        {
            sSendErrorResponse(MODBUS_WRITE_FAILED);
        }
        return;
    }

    stModbusTx.Index = 0;
    for(Cnt = 0; Cnt < 6; Cnt++)
    {
        stModbusTx.Buffer[stModbusTx.Index++] = stModbusRx.Buffer[Cnt];
    }

    //计算回复数据的CRC
    Crc = sModbusGetCrc(stModbusTx.Buffer,6);
    stModbusTx.Buffer[stModbusTx.Index++] = Crc & 0xFF;
    stModbusTx.Buffer[stModbusTx.Index++] = Crc >> 8;

    //计算总的发送长度
    stModbusTx.Length = stModbusTx.Index;

    //发送BUFFER中的首个数据，其他在中断中发送
    stModbusTx.Index = 0;
    sModbusSendData();
    
}


/*******************************************************************************
* Function Name  : sModbusClrRxBuffer
* Description    : after process the Reveive data, clear the modbus RX BUFFER
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static void sModbusClrRxBuffer(void)
{       
    u16 Len = 0;
    for(Len = 0; Len < cModRxBusSizeMax; Len++)
    {
        stModbusRx.Buffer[Len] = 0xFF;
    }
}


/*******************************************************************************
* Function Name  : sModbusClrRxBuffer
* Description    : after process the Reveive data, clear the modbus RX BUFFER
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
static void sModbusClrTxBuffer(void)
{
    stModbusTx.Length = 0;
    stModbusTx.Index = 0;
}

void sHandleInvalidFuncCode(void)
{
    u16 Crc;
    u16 Index;
    if(stModbusRx.Index >= 2)
    {
        Index = stModbusRx.Index-2;
        Crc =((u16)stModbusRx.Buffer[Index+1] << 8) + (stModbusRx.Buffer[Index]);
        if(Crc == sModbusGetCrc(stModbusRx.Buffer,Index))
        {
            sSendErrorResponse(MODBUS_INVALID_FUNC_CODE);
            sModbusClrRxBuffer();            
        }        
    }    
}


/*******************************************************************************
* Function Name  : sModbusParse
* Description    : Parse the data from the modbus
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void sModbusParse(void)
{
    u16 ubChksumIdx;
    u16 Crc = 0;
    u16 OverTimeCnt;
    if (stModbusRx.Index >= cModRxBusSizeMax)
    {
        stModbusRx.Index = 0;
    }

    if(stModbusRx.Buffer[0] != pApp->OwnAddr) 
    {   
        if(stModbusRx.Buffer[0] != cBroadcastAddr)
        {
            return;
        }
    }

     switch(stModbusRx.Buffer[1])
    {
        case MODBUS_03_READ:
        case MODBUS_04_READ:
            ubChksumIdx = 6;
            break;
        case MODBUS_SINGLE_WRITE:
            ubChksumIdx = 6;
            break;
        case MODBUS_MULTI_WRITE:
            ubChksumIdx = stModbusRx.Buffer[6]+7;
            break;
        default:
            sHandleInvalidFuncCode();
            return;
    }
    if(ubChksumIdx+1 >= cModRxBusSizeMax) return;

    Crc =((uint16_t)stModbusRx.Buffer[ubChksumIdx+1] << 8) + (stModbusRx.Buffer[ubChksumIdx]);

    //比较CRC，如不相等，则数据错误
    if(Crc != sModbusGetCrc(stModbusRx.Buffer,ubChksumIdx))
    {
        return;
    }

    OverTimeCnt = 0;
    while(1)
    {
        if(pApp->pAppIsBusy())
        {
            pApp->pWaitSomeTime();
            OverTimeCnt++;
            if(OverTimeCnt > (pApp->Overtime/pApp->waitingTime)) break;
        }
        else
        {
            break;
        }
    }
    if(OverTimeCnt <= (pApp->Overtime/pApp->waitingTime))   
    {    
        switch(stModbusRx.Buffer[1])
        {
            case MODBUS_03_READ:
                sModbusParse0304ReadCmd(MODBUS_03_READ);
                break;
            case MODBUS_04_READ:
                sModbusParse0304ReadCmd(MODBUS_04_READ);
                break;
            case MODBUS_SINGLE_WRITE:
                sModbusParseSingleWriteCmd();
                break;
            case MODBUS_MULTI_WRITE:
                sModbusParseMultiWriteCmd();
                break;
            default:
                break;
        }
    }
    else
    {
        sSendErrorResponse(MODBUS_DEVICE_BUSY);
    }
    sModbusClrRxBuffer();
}

void sChkModbusComm(void)
{
    if(RxTimeOutCnt < pApp->TimeOutSet)
    {  
        RxTimeOutCnt++;
    }
    else if(RxTimeOutCnt == pApp->TimeOutSet)
    {
        RxTimeOutCnt++;
        stModbusRx.Index = 0;
    }

    if(TxTimeOutCnt < pApp->TimeOutSet)
    {  
        TxTimeOutCnt++;
    }
    else if(TxTimeOutCnt == pApp->TimeOutSet)
    {
        TxTimeOutCnt++;
        sModbusClrTxBuffer();
        sModbusEnableReceive();
    }
}

void sSetModbusAddr(u8 addr)
{
    pApp->OwnAddr = addr;    
}


/******************************end of modubus.c**********************************/























