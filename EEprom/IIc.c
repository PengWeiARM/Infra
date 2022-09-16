#include "IIc.h"

volatile i2cErr_t i2cState;
u8 i2cDataNumToRx;
static IIcInit_t *pApp;

void sInitI2c(IIcInit_t *pInit)
{
    pApp = pInit;
    i2cState = i2c_idle;
    //SCL_OUT();
    SDA_OUT();
}

void sTestIIC(void)
{
	u8 RxTest[20];
	static u8 TxTest[]="test eeprom         ";
	eepromWrite(0xa0,0,20,(u16 *)TxTest);
	eepromRead(0xa0,0,20,(u16 *)RxTest);
}

void i2cStart(void)
{
    SDA_OUT();
    pApp->pDelayUs(2);
    SDA_HIGH(); 
    SCL_HIGH(); 
    pApp->pDelayUs(2);
    SDA_LOW();
    pApp->pDelayUs(2);
    SCL_LOW();
}
void i2cStop(void)
{
    SDA_OUT();
    pApp->pDelayUs(2);
    SCL_LOW();
    SDA_LOW();
    pApp->pDelayUs(2);
    SCL_HIGH(); 
    pApp->pDelayUs(2);
    SDA_HIGH(); 

}
bool_t i2cWaitAck(void)
{
    u16 errtime = 255;

    SDA_OUT();

    SDA_HIGH();
    pApp->pDelayUs(2);
    SDA_IN();
    SCL_LOW();
    pApp->pDelayUs(2);
    SCL_HIGH();
    pApp->pDelayUs(2);
    while(SDA) 
    {
        errtime--; 
        if (!errtime) 
        {
            i2cStop();
            return false;
        }
    }
    SCL_LOW();
    return true;
}
void i2cSendAck(void)
{
    SDA_OUT();
    SDA_LOW(); 
    pApp->pDelayUs(2);
    SCL_HIGH(); 
    pApp->pDelayUs(2);
    SCL_LOW();
}

void i2cSendNAck(void)
{
    SDA_OUT();
    SDA_HIGH(); 
    pApp->pDelayUs(2);
    SCL_HIGH();
    pApp->pDelayUs(2);
    SCL_LOW();
}
void i2cSendByte(u8 ch)
{
    u8 i = 8;

    SDA_OUT();
    while (i--)
    {
        SCL_LOW();
        pApp->pDelayUs(2);
        if(ch & 0x80)
        {
            SDA_HIGH();
        }
        else
        {
            SDA_LOW();
        }
        ch <<= 1; 
        pApp->pDelayUs(2);
        SCL_HIGH(); 
        pApp->pDelayUs(2);
        SCL_LOW();
    }
    SCL_LOW();
}

u8 i2cRecByte(void)
{
    u8 i=8;
    u8 ddata=0;
    SDA_IN();
    SDA_HIGH();
    while (i--)
    {
        ddata <<= 1;
        SCL_LOW();pApp->pDelayUs(2);
        SCL_HIGH();pApp->pDelayUs(2);
		if(SDA)ddata |= 1;
    }
    SCL_LOW();
    SDA_OUT();
    return ddata;
}

i2cErr_t eepromWrite(u8 slaveAddr, u16 offset, u16 len, u16* data)
{
    u16 writeAdr;
    u16 endAdr;
    u16 byteLen;
    u16 writeLen;

    if(len == 0)return i2c_success;

    byteLen =  len<<1;
    writeAdr = offset;
    endAdr  = offset + byteLen;

    while(writeAdr!=endAdr)
    {
    	writeLen = (writeAdr/PAGE_SIZE+1)*PAGE_SIZE - writeAdr;
    	if(writeLen < byteLen)
    	{
    		eepromPageWrite(slaveAddr,writeAdr,writeLen>>1,data);
    		data += (writeLen>>1);
    		byteLen -= writeLen;
    		writeAdr += writeLen;
    	}
    	else
    	{
    		eepromPageWrite(slaveAddr,writeAdr,byteLen>>1,data);
    		writeAdr += byteLen;
    	}
    }

    i2cState = i2c_idle;
    return i2c_success;
}


i2cErr_t eepromRead(u8 slaveAddr, u16 offset, u16 len, u16* data)
{
    u16 readAdr;
    u16 endAdr;
    u16 byteLen;
    u16 readLen;

    if(len == 0)return i2c_success;

    byteLen =  len<<1;
    readAdr = offset;
    endAdr  = offset + byteLen;

    while(readAdr!=endAdr)
    {
    	readLen = (readAdr/PAGE_SIZE+1)*PAGE_SIZE - readAdr;
    	if(readLen < byteLen)
    	{
    		eepromPageRead(slaveAddr,readAdr,readLen>>1,data);
    		data += (readLen>>1);
    		byteLen -= readLen;
    		readAdr += readLen;
    	}
    	else
    	{
    		eepromPageRead(slaveAddr,readAdr,byteLen>>1,data);
    		readAdr += byteLen;
    	}
    }

    i2cState = i2c_idle;
    return i2c_success;
}

/********************
len: in word -- offset¡êoaddr of byte
************************/
i2cErr_t eepromPageWrite(u8 slaveAddr, u16 offset, u16 len, u16* data)
{
    u16 lenTmp;
    if(len == 0)return i2c_success;
    if(i2cState != i2c_idle)return i2c_fail;
    offset &= 0xFFFE; //must be even
    len <<= 1; //change to byte
    i2cState = i2c_write_busy;
    do
    {
        lenTmp = EEPROM_PAGESIZE - (offset & (EEPROM_PAGESIZE - 1));
        if(lenTmp > len)lenTmp = len;
        len -= lenTmp;

#ifndef EEPROM_16BIT_ADR
        slaveAddr |= ((offset>>7)&0x06);
#endif

        i2cStart();
        i2cSendByte(slaveAddr );
        i2cWaitAck();

#ifdef EEPROM_16BIT_ADR
        i2cSendByte((offset>>8) & 0x1F);
        i2cWaitAck();
#endif
        i2cSendByte(offset & 0xFF);
        i2cWaitAck();

        offset += lenTmp;

        while (lenTmp)
        {
            lenTmp -= 2;
            i2cSendByte(*data & 0xFF);
            i2cWaitAck();
            i2cSendByte((*data >> 8) & 0xFF);
            i2cWaitAck();
            data++;
        }
        i2cStop();

        pApp->pDelayUs(500);
        pApp->pDelayUs(500);
        pApp->pDelayUs(500);
        pApp->pDelayUs(500);

    }while(len);
  
    i2cState = i2c_idle;
    return i2c_success;
}

/********************
len: in word
************************/
i2cErr_t eepromPageRead(u8 slaveAddr, u16 offset, u16 len, u16* data)
{
    u16 lenTmp;
    if(len == 0)return i2c_success;
    if(i2cState != i2c_idle)return i2c_fail;
    offset &= 0xFFFE; //must be even
    len <<= 1; //change to byte

    i2cState = i2c_read_busy;
    do
    {

        lenTmp = EEPROM_PAGESIZE - (offset & (EEPROM_PAGESIZE - 1));
        if(lenTmp > len)lenTmp = len;
        len -= lenTmp;

#ifndef EEPROM_16BIT_ADR
        slaveAddr |= ((offset>>7)&0x06);
#endif

        i2cStart();
        i2cSendByte(slaveAddr);
        i2cWaitAck();

#ifdef  EEPROM_16BIT_ADR
        i2cSendByte((offset>>8) & 0x1F);
        i2cWaitAck();
#endif
        i2cSendByte(offset & 0xFF);
        i2cWaitAck();

        offset += lenTmp;
        i2cStart();
        i2cSendByte(slaveAddr | 1);
        i2cWaitAck();

        while (lenTmp)
        {
            lenTmp -= 2;
            *data = i2cRecByte();
            i2cSendAck();
            *data += ((u16)i2cRecByte() << 8);
            if(lenTmp != 0)i2cSendAck();
            data++;
        }
        i2cSendNAck();
        i2cStop();
    }while(len);

    i2cState = i2c_idle;
    return i2c_success;
}


//void sDelayUs(uint32_t uwDlyCnt)
//{
//    while(uwDlyCnt--)
//    {
//        asm(" RPT #150 || NOP");
//    }
//}


//
//i2cErr_t rtcRegWrite(uint8_t slaveAddr, uint8_t reg, uint8_t value)
//{
//
//    if(i2cState != i2c_idle)return i2c_fail;
//    i2cState = i2c_write_busy;
//
//    i2cStart();
//    i2cSendByte(slaveAddr);
//    i2cWaitAck();
//
//    i2cSendByte(reg);
//    i2cWaitAck();
//
//    i2cSendByte(value);
//    i2cWaitAck();
//
//    i2cStop();
//    i2cState = i2c_idle;
//    return i2c_success;
//}
//
//i2cErr_t rtcRegRead(uint8_t slaveAddr, uint8_t reg,  uint8_t* value)
//{
//
//    if(i2cState != i2c_idle)return i2c_fail;
//    i2cState = i2c_read_busy;
//
//    i2cStart();
//    i2cSendByte(slaveAddr);
//    i2cWaitAck();
//
//    i2cSendByte(reg);
//    i2cWaitAck();
//
//    i2cStart();
//    i2cSendByte(slaveAddr | 1);
//    i2cWaitAck();
//
//    *value = i2cRecByte();
//    i2cSendNAck();
//
//    i2cStop();
//    i2cState = i2c_idle;
//
//    return i2c_success;
//}

