//******************************************************************************
//*
//*           Copyright (c) 2015 xx-xx-xx Corporation
//*                  ALL RIGHTS RESERVED
//*
//******************************************************************************

//******************************************************************************
//*  $Id: Flash.c 1732 2015-06-08 03:42:49Z  $
//*
//*Description:
//******************************************************************************
#define FLASH_GLOBLS

#include "Flash.h"
#include "FlashApp.h"
#include "main.h"

static flashInit_t *pApp;

void Message_Trans(int word);



void sInitFlash(flashInit_t *pInit)
{
    pApp = pInit;
#if 0			//SPI:WP��HOLD��Ӳ������
    pApp->pHoldPinCtrl(HighLevel);
    pApp->pWpPinCtrl(HighLevel);
#endif
}


/* flash��д */
u8 SPIx_ReadWriteByte(u8 TxData)
{
    while(pApp->pIsSending());  //�鷢�ͻ������Ƿ�Ϊ�գ��ռ����Է���
    pApp->pSendData(TxData);	//����һ���ֽ�
//		sUartSendData(eExternComm, TxData);
    //��SPI���ջ�����Ϊ��ʱ�ȴ�
    while(pApp->pIsReceiving());
    return pApp->pReceiveData();
}

/*******************************************************************************
* Function Name  :  spiDelayUs
* Description    :  delay for some us time
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void spiDelayUs(u16 us)
{
    pApp->pDelayUs(us);
}

/* SPIƬѡ */
void flashCsAssert(void)
{
    pApp->pCsPinCtrl(LowLevel);
}
void flashCsDeAssert(void)
{
    pApp->pCsPinCtrl(HighLevel);
}


/*******************************************************************************
* Function Name  :  flashWaitBusy
* Description    :  wait until erase or program is done
��ȡbusy��־λ
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashWaitBusy(void)
{
    u32 i = 0;
    u16 data;

    while(i++ < 100000)
    {
        flashCsAssert();
//      	spiTxOneByte(flash_read_status_reg1);
        SPIx_ReadWriteByte(flash_read_status_reg1);
        data = 0;                                  
        data = SPIx_ReadWriteByte(0xff);		//spiRxOneByte();
        flashCsDeAssert();
		if((data & 0x1) == 0)return;
        spiDelayUs(10);
    }
}

/*******************************************************************************
* Function Name  :  flashChipErase
* Description    :  erase the whole chip
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashWaitBusy1(void)
{
    u16 data;
    while(1)
    {
        flashCsAssert();
// 				spiTxOneByte(flash_read_status_reg1);
        SPIx_ReadWriteByte(flash_read_status_reg1);
        data = 0;
        data = SPIx_ReadWriteByte(0xff);		//spiRxOneByte();
        flashCsDeAssert();
        if((data & 0x1) == 0)
        {
            break;
        }
        //pApp->pDelayPend();
    }
}

/*******************************************************************************
* Function Name  :  flashChipErase
* Description    :  erase the whole chip
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashChipErase(void)
{
    flashCsAssert();
    SPIx_ReadWriteByte(flash_write_enable);
    flashCsDeAssert();
    spiDelayUs(5);	
    flashCsAssert();
		SPIx_ReadWriteByte(flash_chip_erase);
    flashCsDeAssert();
    flashWaitBusy1();

}
/*******************************************************************************
* Function Name  :  flashErase
* Description    :  erase one sector specified by address
* Input          : None
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashErase(u32 sectorAddr)
{
    flashCsAssert();
    SPIx_ReadWriteByte(flash_write_enable);
    flashCsDeAssert();
    spiDelayUs(5);		
    flashCsAssert();
    SPIx_ReadWriteByte(flash_sector_erase);
    SPIx_ReadWriteByte((sectorAddr >> 16));
    SPIx_ReadWriteByte((sectorAddr >> 8));
    SPIx_ReadWriteByte(sectorAddr);
    flashCsDeAssert();
    flashWaitBusy();

}

/*******************************************************************************
* Function Name  :  flashRead
* Description    :  read data from flash
* Input          :
						addr: 	data address
						len: 		length of 16-bit data
						data: 	space for data to be stored.
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashRead(u32 addr, u16 len, u16* data)
{
    u16 realLen;

    realLen = FLASHPAGESIZE - (addr & (FLASHPAGESIZE - 1));
    len <<= 1;
    if(realLen > len)realLen = len;

    while(len > 0)
    {
        len -= realLen;

				flashCsAssert();								//����Ƭѡ����ʼ��ȡ------test:ok
				SPIx_ReadWriteByte(flash_read_data);
				SPIx_ReadWriteByte((addr >> 16));
				SPIx_ReadWriteByte((addr >> 8));
				SPIx_ReadWriteByte(addr);

        addr += realLen;

        while(realLen)
        {
            *data = SPIx_ReadWriteByte(0xff);
            *data += (u16)SPIx_ReadWriteByte(0xff)<<8;
            data++;
            realLen -= 2;
        }
        flashCsDeAssert();		//����Ƭѡ

        if(len > FLASHPAGESIZE)					//------------test:false
        {
            realLen = FLASHPAGESIZE;
        }
        else
        {
            realLen = len;
        }
    }
}

/*******************************************************************************
* Function Name  :  flashVerify
* Description    :  ��addrָ���ĵط���ȡlen���ֵ����ݣ���data���бȽϣ����Ƿ�һ��
* Input          : 
						addr:		flash data address
						len:		length of 16-bit data
						data: 	data given to be verified
* Output         : None.
* Return         : 0:verify fail  1: verify OK.
*******************************************************************************/
bool flashVerify(u32 addr, u16 len, u16* data)
{
    u16 tmp;
    while(len--)
    {
        flashRead(addr, 1, &tmp);			
        if(tmp != *data)
        {
            return FALSE;
        }
        data++;
        addr += 2;
    }
    return TRUE;
}

/*******************************************************************************
* Function Name  :  flashDirectProgram
* Description    :  ֱ�ӽ�����д��flash����������������
* Input          : 
						addr: 	data address
						len:		length of 16-bit data
						data:  	data to be programmed.
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashDirectProgram(u32 addr, u16 len, u16* data)
{
    u16 realLen;

    realLen = FLASHPAGESIZE - (addr & (FLASHPAGESIZE - 1));
    len <<= 1;
    if(realLen > len)realLen = len;

    while(len > 0)
    {
        len -= realLen;

        flashCsAssert();
				SPIx_ReadWriteByte(flash_write_enable);
				flashCsDeAssert();
				spiDelayUs(5);		
				flashCsAssert();	
				SPIx_ReadWriteByte(flash_page_program);
				SPIx_ReadWriteByte(addr >> 16);
				SPIx_ReadWriteByte(addr >> 8);
				SPIx_ReadWriteByte(addr);
		
        addr += realLen;
        while(realLen)
        {
						SPIx_ReadWriteByte(*data);
						SPIx_ReadWriteByte(*data>>8);
            data++;
            realLen -= 2;
        }
        flashCsDeAssert();
        flashWaitBusy();
        if(len > FLASHPAGESIZE)
        {
            realLen = FLASHPAGESIZE;
        }
        else
        {
            realLen = len;
        }
    }
    
}

/*******************************************************************************
* Function Name  :  flashDirectProgramWithCheck
* Description    : 	ֱ�ӽ�����д��flash������������������д��ǰ�����齫Ҫ�����flash�Ƿ�Ϊ��
* Input          : 
						addr: 	data address
						len:		length of 16-bit data
						data:  	data to be programmed.
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashDirectProgramWithCheck(u32 addr, u16 len, u16* data)
{
    u16 len1;
    u16 tmp;
    len1 = len << 1;
    while(len1)
    {
        flashRead(addr + len1 - 2, 1, &tmp);
        if(tmp != 0xFFFF)return;
        len1 -= 2;
    }
    flashDirectProgram(addr, len, data);

}

/*******************************************************************************
* Function Name  :  flashProgramDataitem
* Description    : ������д��ָ���Ŀ�sector������д��ǰ��Ҫ���в�����Ϊ�˷�ֹ���綪ʧ����
									 ����һ�������� (swap��sector0)  ��Ϊ��ʱ���ݴ�ŵ㡣�������ĸ�ʽ��������������
		||word0							||word1,word2		 	||word3			|| word4...			||
		||0x55AA ��ʱ��־λ	||����Ҫд��ĵ�ַ	||���ݳ���		|| Ҫд�������	||
* Input          :
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashProgramDataitem(u32 addr, u16 len, u32 SectorStartAddr, u32 SectorEndAddr, u16* data)
{
    u16 tmp[7];
    u32 tempAddr;
    u32 tempSectorNumber;
    u32 SectorStartNumber;
    u32 SectorEndNumber;
    if(addr < SectorStartAddr) return;
    tempAddr = addr + 2*len;
    if(tempAddr > SectorEndAddr) return;

    /* �ֽ�����д�뻺���� */
    flashErase(0);
    tmp[0] = 0x55AA;
    tmp[1] = addr & 0xFFFF;
    tmp[2] = addr >> 16;
    tmp[3] = SectorStartAddr & 0xFFFF;
    tmp[4] = SectorStartAddr >> 16;
    tmp[5] = SectorEndAddr & 0xFFFF;
    tmp[6] = SectorEndAddr >> 16;
    flashDirectProgram(0, 7, tmp);
    flashDirectProgram(14, 1, &len);
    flashDirectProgram(16, len, data);// write to swap area first

    /* ������д����ʽ���� */
    SectorStartNumber = SectorStartAddr/FLASHSECTORSIZE;
    SectorEndNumber = SectorEndAddr/FLASHSECTORSIZE;
    for(tempSectorNumber = SectorStartNumber; tempSectorNumber <= SectorEndNumber; tempSectorNumber++)
    {
        flashErase(tempSectorNumber * FLASHSECTORSIZE);
    }
    flashDirectProgram(addr, len, data);

    //�����ʱ��־λ
    tmp[0] = 0;
    flashDirectProgram(0, 1, tmp);
}


/*******************************************************************************
* Function Name  :  flashReadUniqueId
* Description    :  ��ȡflash��uniqueID
* Input          :
* Output         : None.
* Return         : None.
*******************************************************************************/
void flashReadUniqueId(u16* data)
{
    u8 i = 0;

    flashCsAssert();
		SPIx_ReadWriteByte(flash_read_unique_id_number);
		SPIx_ReadWriteByte(0xFF);
		SPIx_ReadWriteByte(0xFF);
		SPIx_ReadWriteByte(0xFF);
		SPIx_ReadWriteByte(0xFF);
	
    while(i++ < 4)
    {
        *data = SPIx_ReadWriteByte(0xFF);//spiRxOneByte();
        *data = (*data << 8) + SPIx_ReadWriteByte(0xFF);//spiRxOneByte();
        data++;
    }
    flashCsDeAssert();
}

void flashPowerOn(void)
{
    flashCsAssert();
    spiDelayUs(500);
    spiDelayUs(500);
    spiDelayUs(500);
    spiDelayUs(500);
    flashCsDeAssert();
}



