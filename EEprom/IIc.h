#ifndef __I2C_H__
#define __I2C_H__


#include "typedef.h"
#include "macro.h"

typedef struct
{
    void (*pDelayUs)(u32 us);
}IIcInit_t;

#define cI2CAdr	0xA0

//#define c24C02
//#define c24C04
//#define c24C08
//#define c24C16
#define c24C32
//#define c24C64

#ifdef c24C02
#define EEPROM_PAGESIZE 8
#endif

#ifdef c24C04
#define EEPROM_PAGESIZE 16
#endif

#ifdef c24C08
#define EEPROM_PAGESIZE 16
#define MAX_ADDR	1024
#define PAGE_SIZE	256
#endif

#ifdef  c24C16
#define EEPROM_PAGESIZE 16
#endif


#ifdef  c24C32
#define EEPROM_16BIT_ADR
#define EEPROM_PAGESIZE 32
#define PAGE_SIZE	256
#endif

#ifdef  c24C64
#define EEPROM_16BIT_ADR
#define EEPROM_PAGESIZE 32
#endif



#define SDA     GPIO_SDA
#define SDA_IN()   GPIO_SDA_IN()
#define SDA_OUT()  GPIO_SDA_OUT()


#define SDA_HIGH() GPIO_SDA_HIGH()
#define SDA_LOW()  GPIO_SDA_LOW()

#define SCL_HIGH() GPIO_SCL_HIGH()
#define SCL_LOW()  GPIO_SCL_LOW()

#define NOPs() sDelayUs(2)
//#define NOPs() 




typedef enum
{
    i2c_success,
    i2c_fail,
    i2c_write_busy,
    i2c_read_busy,
    i2c_idle
}i2cErr_t;
void sInitI2c(IIcInit_t *pInit);
void i2cStart(void);
void i2cStop(void);
bool_t i2cWaitAck(void);
void i2cSendAck(void);
void i2cSendNAck(void);
void i2cSendByte(u8 ch);
u8 i2cRecByte(void);

extern i2cErr_t rtcRegRead(u8 slaveAddr,u8 reg,u8 * value);
extern i2cErr_t rtcRegWrite(u8 slaveAddr,u8 reg,u8 value);
extern i2cErr_t eepromRead(u8 slaveAddr,u16 offset,u16 len,u16 * data);
extern i2cErr_t eepromWrite(u8 slaveAddr,u16 offset,u16 len,u16 * data);
extern i2cErr_t eepromPageRead(u8 slaveAddr, u16 offset, u16 len, u16* data);
extern i2cErr_t eepromPageWrite(u8 slaveAddr, u16 offset, u16 len, u16* data);
extern void sInitI2c(IIcInit_t *pInit);

#endif

