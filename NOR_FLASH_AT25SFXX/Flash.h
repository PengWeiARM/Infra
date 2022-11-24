#ifndef __FLASH_H__
#define __FLASH_H__

#ifndef FLASH_GLOBLS
#define FlashEXT extern 
#else
#define FlashEXT
#endif
#include "typedef.h"
#include "macro.h"



//******************************************************************************
//*                 the constant definition
//******************************************************************************
#define FLASHPAGESIZE 0x100   //256
#define FLASHSECTORSIZE 0x1000   //4096


/* W25Q64√¸¡Ó */
typedef enum
{
    flash_write_enable = 0x06,
    flash_write_disable = 0x04,
    flash_read_status_reg1 = 0x05,
    flash_read_status_reg2 = 0x35,
    flash_write_status_reg = 0x01,
    flash_page_program = 0x02,
    flash_sector_erase = 0x20,
    flash_chip_erase = 0xC7,
    flash_erase_program_suspend = 0x75,
    flash_erase_program_resume = 0x7A,
    flash_read_data = 0x03,
    flash_get_manufacturer_device_id = 0x90,
    flash_read_unique_id_number = 0x4B
}flashInstructions_t;

typedef enum
{
   HighLevel  = 0x01,
   LowLevel   = 0x02,
}flashIOLevel_t;



typedef struct
{    
    void (*pDelayUs)(u32 us);
    void (*pSendData)(u8 data);
    u8  (*pReceiveData)(void);
    bool (*pIsSending)(void);
    bool (*pIsReceiving)(void);
    void (*pCsPinCtrl)(flashIOLevel_t level);
    void (*pHoldPinCtrl)(flashIOLevel_t level);
    void (*pWpPinCtrl)(flashIOLevel_t level);
    void (*pDelayPend)(void);

}flashInit_t;


//******************************************************************************
//*                 the function declarations
//******************************************************************************
extern void flashInit(void);
extern void flashRead(u32 addr, u16 len, u16 * data);
extern void flashDirectProgram(u32 addr, u16 len, u16 * data);
extern void flashDirectProgramWithCheck(u32 addr, u16 len, u16 * data);
extern void flashErase(u32 sectorAddr);
extern void flashProgramDataitem(u32 addr, u16 len, u32 SectorStartAddr, u32 SectorEndAddr, u16* data);
extern void flashChipErase(void);
extern bool flashVerify(u32 addr, u16 len, u16* data);
extern void flashReadUniqueId(u16* data);
extern void sInitFlash(flashInit_t *pInit);
extern void flashPowerOn(void);


#endif



