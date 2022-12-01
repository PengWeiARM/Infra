#ifndef __MODBUS_H__
#define __MODBUS_H__

//******************************************************************************
//*                 the constant definition
//******************************************************************************
#include "main.h"

#define cModBusByteMax 255

#define cModRxBusSizeMax (cModBusByteMax+9)
#define cModTxBusSizeMax (cModBusByteMax+5)



#define MODBUS_STATUS_PROCESS 0
#define MODBUS_STATUS_OVER    1

#define MODBUS_03_READ 0x03
#define MODBUS_04_READ 0x04
#define MODBUS_SINGLE_WRITE 0x06
#define MODBUS_MULTI_WRITE   0x10

#define MODBUS_INVALID_FUNC_CODE 0x01
#define MODBUS_INVALID_DATA_ADDR 0x02
#define MODBUS_INVALID_DATA_VALUE 0x03
#define MODBUS_INVALID_DATA_LENTH 0x04
#define MODBUS_WRITE_FAILED 0x05
#define MODBUS_DEVICE_BUSY  0x06
#define MODBUS_INTERNAL_ERROR   0x0A




#define cModbusIdle 0 
#define cModbusBusy 1


//******************************************************************************
//*                 the  global variable  data type define
//******************************************************************************

typedef enum{
    eInvalidDataAddr,
	eInvalidDataValue,
	eNoError,
}rwErrorType_t;


typedef enum{
    e03Read,
    e04Read,
	eSingleWrite,
	eMultiWrite,
}permission_t;

#define P03R_XXW  (1<<e03Read)
#define P04R_XXW  (1<<e04Read)
#define P03R_06W  ((1<<e03Read)|(1<<eSingleWrite))
#define P04R_06W  ((1<<e04Read)|(1<<eSingleWrite))
#define P04R_10W  ((1<<e04Read)|(1<<eMultiWrite))
#define P03R_10W  ((1<<e03Read)|(1<<eMultiWrite))
#define PXXR_06W  (1<<eSingleWrite)
#define PXXR_10W  (1<<eMultiWrite)



typedef enum{
    eSigned,
	eUnsigned,
	eAscII,
}RegDatatype_t;

typedef enum{
    eUsed,
	eReserved,
}RegType_t;





typedef struct
{
    u16 RegAddr;
	void *pData;
    RegDatatype_t RegDatatype;
    RegType_t Regtype;
    u16 permission;
    s16 min;
    s16 max;
    bool (*pfunction)(u16 offset, u16 len, rwErrorType_t *pError);
}modbusRegItem_t;



typedef struct
{
    u8  OwnAddr;
    u16 TimeOutSet;
    void (*pEnableSendSwich)(void);
    void (*pEnableReceiveSwich)(void);
    u16 RegStartAddr;
    u16 RegEndAddr;
    const modbusRegItem_t *pModbusReg;
    u16 RegNum;
    void (*pUartSend)(u8 data);
    bool (*pAppIsBusy)(void);
    void (*pWaitSomeTime)(void);
    u16 waitingTime;
    u16 Overtime;
}modbusInit_t;


//define the receive struct
typedef struct
{
    u16 Index;
    u8  Buffer[cModRxBusSizeMax];
}modbusrx_t;


//define the Tx struct
typedef struct
{
    u16 Length;
    u16 Index; 
    u8 Buffer[cModTxBusSizeMax];
}modbustx_t; 



//define serial baud rate
typedef enum
{
    BAUDRATE_2400 = 0,
    BAUDRATE_4800,
    BAUDRATE_9600,
    BAUDRATE_19200,
    BAUDRATE_38400,
    BAUDRATE_57600,
    BAUDRATE_115200,
}uartbaudrate_t;


//******************************************************************************
//*                 the  global variable declarations
//******************************************************************************



//******************************************************************************
//*                 the function declarations
//******************************************************************************
extern void sModbusInit(modbusInit_t *pInit);
extern void sModbusParse(void);
extern void sModbusReceiveData(u8 Data);
extern void sModbusSendData(void);
extern void sModbusEnableReceive(void);
extern bool sModbusIsSendEnd(void);
extern void sChkModbusComm(void);

#endif

    
    
    
    
     
    




