#ifndef ModbusMaster_H
#define ModbusMaster_H



#define cModbusMasterByteMax 255

#define cModbusMasterRxSizeMax (cModbusMasterByteMax+9)
#define cModbusMasterTxSizeMax (cModbusMasterByteMax+5)

#define MODBUS_M_03_READ 0x03
#define MODBUS_M_04_READ 0x04
#define MODBUS_M_SINGLE_WRITE 0x06
#define MODBUS_M_MULTI_WRITE   0x10


#define MODBUS_M_03_READ_ERROR (0x80|MODBUS_M_03_READ)
#define MODBUS_M_04_READ_ERROR (0x80|MODBUS_M_04_READ)
#define MODBUS_M_SINGLE_WRITE_ERROR (0x80|MODBUS_M_SINGLE_WRITE)
#define MODBUS_M_MULTI_WRITE_ERROR (0x80|MODBUS_M_MULTI_WRITE)



typedef struct
{
    u8  OwnAddr;
    u16 TimeOutSet;
    void (*pEnableSendSwich)(void);
    void (*pEnableReceiveSwich)(void);
    u16 RegStartAddr;
    u16 RegEndAddr;
    //const modbusRegItem_t *pModbusReg;
    u16 RegNum;
    void (*pUartSend)(u8 data);
}ModbusMasterInit_t;

typedef enum{
    eNotReceived,
    eFunCodeError,
    eFrameError,
    eBuffOverflow,
    eAddrError,
	eResponseOK,
}ModbusMasterResult_t;


void sModbusMasterInit(ModbusMasterInit_t *pInit);
void sModbusMasterEnableSend(void);
void sModbusMasterReceiveData(u8 Data);
void sModbusMasterSendData(void);

void sChkModbusMasterComm(void);
void sModbusMasterClrRxBuffer(void);
u8 sGetModbusSlaveAddr(void);
u8 sGetModbusSlaveFunCode(void);
u8 sGetSentSlaveAddr(void);
u8 sGetSentFunCode(void);
void sModbusMasterSendCmd(u8 addr, u8 FunCode, u16 RegAddr, u16 RegLenth, u16 *pData);
ModbusMasterResult_t sModbusMasterParse(void);



		
#endif
