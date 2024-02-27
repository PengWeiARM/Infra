#define UPGRADE_GLOBLS

#include "Upgrade.h"

#define cHeaderDataLen 512

#define cModelOffset (4*2+1)
#define cFwTypeOffset (4*2+2)
#define cChipTypeOffset (4*2+3)
#define cFwVerOffset (4*3)

#define cHeaderSizeOffset_1 4
#define cHeaderSizeOffset_2 (cHeaderDataLen-4*3)
#define cHeaderCrcOffset (cHeaderDataLen-4)

#define cAppSizeOffset (4*5)
#define cAppDataCrcOffset (4*6)

#define cHeaderFieldOffset 0
#define cAppFieldOffset cHeaderDataLen

#define cUserUpdate 0x55aa55aa
#define cAppOk 0x55aa55aa55aa55aa

#define cCheckAppOKFlag  0xFFFFFFFFFFFFFFFF
#define cNoCheckAppOKFlag  0x55aa55aa55aa55aa

#define cNoNeedBurnBootFlag  0xFFFFFFFFFFFFFFFF
#define cNeedBurnBootFlag   (0x55aa55aa55aa55aa)

#define CHECK_APP_OK 1
#define BURN_BOOT 1

static void (*pReadByteData)(u32 addr, u16 len, u8* data);
static void (*pWriteWordDataWithCheck)(u32 addr, u16 len, u16* data);
static bool (*pVerifyWordData)(u32 addr, u16 len, u16* data);
static void (*pEraseDataArea)(u32 addr);

#ifdef CHECK_APP_OK
    const u64 NeedCheckAppOk  __attribute__((section(".ARM.__at_0x08011000"))) = cCheckAppOKFlag;
#elif NO_CHECK_APP_OK
    const u64 NeedCheckAppOk  __attribute__((section(".ARM.__at_0x08011000"))) = cNoCheckAppOKFlag;
#else
    #define NeedCheckAppOk 0x08011000
#endif


#ifdef BURN_BOOT
    const u64 BurnBoot  __attribute__((section(".ARM.__at_0x08011010"))) = cNeedBurnBootFlag;
#elif NO_BURN_BOOT
    const u64 BurnBoot  __attribute__((section(".ARM.__at_0x08011010"))) = cNoNeedBurnBootFlag;
#else
    #define BurnBoot 0x08011010
#endif


volatile u32 User_Update __attribute__((section("USER_UPDATE_FLAG"), zero_init));
s32 dwTransferTime=0;

bool bIsBurningBoot = FALSE;

struct
{
    u32 size1;
    u32 size2;
    u32 crc;    
}stBinHead;

struct
{
    u32 size;
    u32 crc;
	u32 BmsCrc;
}stBinApp;

BinFile_t stBinFileProp;
remoteUpgrade_t stRemoteUpgrade;
upgradeData_t  stUpgradeData;
u8 UpgradeActionType;
u8 UpgradeActionResult;
static UpgradeInit_t *pApp;
u32 maxBinFileSize;
u32 preAddr;
u32 preTotal;
u32 RxBinWordSize;

#define cCrcTabLenth (256)
u32 crc_table[cCrcTabLenth];
//#define cCrcBuffLenth (1024)
u8 crcBuff[cCrcBuffLenth];

bool bReqUpgrade = FALSE;
static FirmwareVer_t stBootVer;


void sStrToVer(FirmwareVer_t *pVer, const u8 *pChar, u16 num);


u32 CalculateCrc32(u32 crc, u32 len, u8 *buf);

#define TestCrc32Algo
#ifdef TestCrc32Algo
const u8  u8DataInBuf[512] = {
    0x9C,0xE7,0x9A,0x76,0x00,0x00,0x02,0x00,0x04,0x01,0x01,0x01,0x00,0x0A,0x00,0x0A,0x00,0x03,0x00,0x00,
    0x00,0x01,0xE8,0xD4,0x0C,0x24,0x92,0xC0,0x00,0x00,0x00,0x00,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
    0x00,0x00,0x02,0x00,0x63,0x18,0x65,0x89, /* 501 - 508 */
    0x04,0x08,0x5E,0x0C,                     /* 509 - 512 */
};
    
const u32  CRC_TAB[cCrcTabLenth] = {
    0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3, 0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E,
    0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91, 0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB,
    0xF4D4B551, 0x83D385C7, 0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5, 0x3B6E20C8,
    0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B, 0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940,
    0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59, 0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599,
    0xB8BDA50F, 0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D, 0x76DC4190, 0x01DB7106,
    0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433, 0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB,
    0x086D3D2D, 0x91646C97, 0xE6635C01, 0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
    0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65, 0x4DB26158, 0x3AB551CE, 0xA3BC0074,
    0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB, 0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5,
    0xAA0A4C5F, 0xDD0D7CC9, 0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F, 0x5EDEF90E,
    0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD, 0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A,
    0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683, 0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27,
    0x7D079EB1, 0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7, 0xFED41B76, 0x89D32BE0,
    0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5, 0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1,
    0xA6BC5767, 0x3FB506DD, 0x48B2364B, 0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
    0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F, 0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92,
    0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D, 0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F,
    0x72076785, 0x05005713, 0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21, 0x86D3D2D4,
    0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777, 0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C,
    0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45, 0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D,
    0x3E6E77DB, 0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9, 0xBDBDF21C, 0xCABAC28A,
    0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF, 0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37,
    0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D
};


void sDoCRC32Cal(void)
{
    u32 CRC_from_calculation = 0;
    u32 CRC_from_calculation_Out = 0;
    u16 len;
    u16 index;
    u16 realLen;
    
    CRC_from_calculation = 0xffffffffL;
    len = 508;
    index = 0;
    if(len > cCrcBuffLenth)
    {
        realLen = cCrcBuffLenth;
    }
    else
    {
        realLen = len;
    }

    while(len > 0)
    {
        len -= realLen;
        for(u16 j=0; j< realLen; j++)
        {
            crcBuff[j] = u8DataInBuf[index + j];
        }
        CRC_from_calculation = CalculateCrc32( CRC_from_calculation, realLen, crcBuff);
        index += realLen;		
       
        if(len > cCrcBuffLenth)
        {
            realLen = cCrcBuffLenth;
        }
        else
        {
            realLen = len;
        }
    }
    CRC_from_calculation_Out  = CRC_from_calculation;
    CRC_from_calculation_Out ^= 0xffffffffL;
}

#endif

void Smake_crc32_table(void)
{
    u32 c;
    s16 n, k;
    u32 poly;            /* polynomial exclusive-or pattern */
    /* terms of polynomial defining this crc (except x^32): */
    static const u8 p[] = {0,1,2,4,5,7,8,10,11,12,16,22,23,26};

    /* make exclusive-or pattern from polynomial (0xedb88320L) */
    poly = 0L;
    for (n = 0; n < sizeof(p)/sizeof(u8); n++)
      poly |= 1L << (31 - p[n]);

    for (n = 0; n < cCrcTabLenth; n++)
    {
      c = (u32)n;
      for (k = 0; k < 8; k++)
        c = c & 1 ? poly ^ (c >> 1) : c >> 1;
      crc_table[n] = c;
    }

    for(n=0; n<cCrcTabLenth; n++)
    {   
        if(crc_table[n] != CRC_TAB[n])
        {
            break;
        }
        
    }
}

u32 CalculateCrc32(u32 crc, u32 len, u8 *buf)
{
    while (len--)
    {
        crc = crc_table[((s32)crc ^ (s32)(*buf++)) & 0xffL] ^ (crc >> 8);
    }
    return crc;
}

void sReadBootVer(FirmwareVer_t *pVer)
{
    u32 addr;
    addr = sGetBootVerAddr();
    sStrToVer(&stBootVer, (u8*)(addr), 20);
}

void sInitUpgrade(UpgradeInit_t *pInit)
{
    pApp = pInit;
    pReadByteData = pInit->pReadByteData;
    pWriteWordDataWithCheck = pInit->pWriteWordDataWithCheck;
    pVerifyWordData = pInit->pVerifyWordData;
    pEraseDataArea = pInit->pEraseDataArea;
    
    maxBinFileSize = pApp->BinFileEndAddr - pApp->BinFileStartAddr;
    Smake_crc32_table();
    sReadBootVer(&stBootVer);

    #ifdef TestCrc32Algo
     sDoCRC32Cal();
    #endif
}

u8 sRdByeData(u32 offset)
{
    u8 tmp;
    pReadByteData(pApp->BinFileStartAddr + offset, 1, &tmp);
    return tmp;
}

u16 sRdWordData(u32 offset)
{
    u8 tmp[2];
    u16 temp;
    for(u16 i=0; i<2; i++)
    {
        pReadByteData(pApp->BinFileStartAddr + offset + i, 1, &tmp[i]);
    }
    temp  = (u16)(tmp[0]<<8) + tmp[1];
    return temp;
}

u32 sRdDoubleWodData(u32 offset)
{   
    u8 tmp[4];
    u32 temp;
    for(u16 i=0; i<4; i++)
    {
        pReadByteData(pApp->BinFileStartAddr + offset + i, 1, &tmp[i]);
    }
    temp  = (u16)(tmp[0]<<8) + tmp[1];
    temp = temp<<16;
    temp  |= (u16)(tmp[2]<<8) + tmp[3];
    return temp;
}

void sRdFileProp(void)
{   
    stRemoteUpgrade.BinFile.model = (u16)sRdByeData(cModelOffset);
    stRemoteUpgrade.BinFile.FirmwareType = (u16)sRdByeData(cFwTypeOffset);
    stRemoteUpgrade.BinFile.chip = (u16)sRdByeData(cChipTypeOffset);
    stRemoteUpgrade.BinFile.FirmwareVer.Ver.major = (u16)sRdByeData(cFwVerOffset);
    stRemoteUpgrade.BinFile.FirmwareVer.Ver.minor = (u16)sRdByeData(cFwVerOffset + 1);
    stRemoteUpgrade.BinFile.FirmwareVer.Branch = (u16)sRdWordData(cFwVerOffset + 2);
    stRemoteUpgrade.BinFile.FirmwareVer.Tag = (u16)sRdWordData(cFwVerOffset + 4);
}

u16 sGetModelInBin(void)
{
    return stRemoteUpgrade.BinFile.model;
}

u16 sGetFwTypeInBin(void)
{
    return stRemoteUpgrade.BinFile.FirmwareType;
}

u16 sGetChipInBin(void)
{
    return stRemoteUpgrade.BinFile.chip;
}

FWVer_t *sGetFwInBin(void)
{
    return &stRemoteUpgrade.BinFile.FirmwareVer;
}


void sRdBinHeaderSize(void)
{
    stBinHead.size1 = sRdDoubleWodData(cHeaderSizeOffset_1);
    stBinHead.size2 = sRdDoubleWodData(cHeaderSizeOffset_2);
}

void sRdBinHeaderCrc(void)
{
    stBinHead.crc = sRdDoubleWodData(cHeaderCrcOffset);
}

void sRdBinAppSize(void)
{
    stBinApp.size = sRdDoubleWodData(cAppSizeOffset);
}

void sRdBinAppCrc(void)
{
    stBinApp.crc = sRdDoubleWodData(cAppDataCrcOffset);
}



u32 sGetAppSize(void)
{
    return stBinApp.size;
}

u32 sGetAppCrc(void)
{
    return stBinApp.crc;
}

u32 sGetAppAreaSize(void)
{
    return pApp->AppAreaSize;
}

void sSetUpgradeAction(u8 Type)
{
    UpgradeActionType = Type;
}

u8 sGetUpgradeAction(void)
{
    return UpgradeActionType;
}

void sSetUpgradeActionResult(u8 result)
{
    UpgradeActionResult = result;
}

u8 sGetUpgradeActionResult(void)
{
    return UpgradeActionResult;
}

void sResetRxWordCnt(void)
{
    preAddr = 0xFFFFFFFF;
    preTotal = 0;
    RxBinWordSize = 0;
}

bool sEraseFileOkay(void)
{
    u32 StartAddr = pApp->BinFileStartAddr;
    u32 EndAddr = pApp->BinFileEndAddr;
    u32 SectorSize = pApp->FlashSectorSize;
    u32 addr;
    u32 len = EndAddr - StartAddr +1;
    u32 realLen;

    sResetRxWordCnt();
    for(addr= StartAddr; addr <= EndAddr; addr += SectorSize)
    {
        pEraseDataArea(addr);
        pApp->pFedDog();
    }

    addr = StartAddr;
    if(len > cCrcBuffLenth)
    {
        realLen = cCrcBuffLenth;
    }
    else
    {
        realLen = len;
    }

    while(len > 0)
    {   
        pApp->pFedDog();
        len -= realLen;
        pReadByteData(addr, realLen, crcBuff);
        for(u16 i=0; i<realLen; i++)
        {
            if(crcBuff[i] != 0xff) 
            {
                return FALSE;
            }
        }  
        addr += realLen;		
       
        if(len > cCrcBuffLenth)
        {
            realLen = cCrcBuffLenth;
        }
        else
        {
            realLen = len;
        }
    }
    return TRUE;
}

void sSaveFileProp(u16 *pProp)
{
    stBinFileProp.model = pProp[0];
    stBinFileProp.FirmwareType = pProp[1];
    stBinFileProp.chip = pProp[2];
    stBinFileProp.FirmwareVer.Ver.major = pProp[3];
    stBinFileProp.FirmwareVer.Ver.minor = pProp[4];
    stBinFileProp.FirmwareVer.Branch = pProp[5];
    stBinFileProp.FirmwareVer.Tag = pProp[6];    
}

bool sFilePropCheckIsOk(void)
{
    u16 i;
    sRdFileProp();
    if(stRemoteUpgrade.BinFile.model !=  pApp->ownInfor.model) return FALSE;
    if(stRemoteUpgrade.BinFile.FirmwareType == pApp->ownInfor.ZonergyFwChip.FwType)
    {
        for(i=0; i<pApp->ownInfor.ZonergyFwChip.chipNum; i++)
        {
            if(stRemoteUpgrade.BinFile.chip == pApp->ownInfor.ZonergyFwChip.chip[i]) break;
        }
        if(i>=pApp->ownInfor.ZonergyFwChip.chipNum) return FALSE;
    }
    

    if(stRemoteUpgrade.BinFile.FirmwareType == pApp->ownInfor.PaceBms.FwType)
    {
        for(i=0; i<pApp->ownInfor.PaceBms.chipNum; i++)
        {
            if(stRemoteUpgrade.BinFile.chip == pApp->ownInfor.PaceBms.chip[i]) 
            {
                break;
            }
          /*  if (stRemoteUpgrade.BinFile.chip == WEB_BINFILE_CHIP)
            {
                break;
            }*/
        }
        if(i>=pApp->ownInfor.PaceBms.chipNum) return FALSE;
    }    
    
    if(stRemoteUpgrade.BinFile.model != stBinFileProp.model) return FALSE;
    if(stRemoteUpgrade.BinFile.FirmwareType != stBinFileProp.FirmwareType) return FALSE;
    if (stRemoteUpgrade.BinFile.chip != stBinFileProp.chip) 
    {
        if (stRemoteUpgrade.BinFile.chip != WEB_BINFILE_CHIP)
        {
            return FALSE;
        }
    }
    if(stRemoteUpgrade.BinFile.FirmwareVer.Ver.major != stBinFileProp.FirmwareVer.Ver.major) return FALSE;
    if(stRemoteUpgrade.BinFile.FirmwareVer.Ver.minor != stBinFileProp.FirmwareVer.Ver.minor) return FALSE;
    if(stRemoteUpgrade.BinFile.FirmwareVer.Branch != stBinFileProp.FirmwareVer.Branch) return FALSE;
    if(stRemoteUpgrade.BinFile.FirmwareVer.Tag != stBinFileProp.FirmwareVer.Tag) return FALSE;
    return TRUE;
}

u32 sCalBinDataCrc32(u32 addr, u32 len)
{
    u32 CRC_from_calculation = 0;
    u32 CRC_from_calculation_Out = 0;
    u32 realLen;
    if(addr%2 != 0) return 0;
    if(len%2 != 0) return 0;
    
    CRC_from_calculation     = 0xffffffffL;
    if(len > cCrcBuffLenth)
    {
        realLen = cCrcBuffLenth;
    }
    else
    {
        realLen = len;
    }

    while(len > 0)
    {   
        pApp->pFedDog();
        len -= realLen;
        pReadByteData(addr, realLen, crcBuff);
        CRC_from_calculation = CalculateCrc32( CRC_from_calculation, realLen, crcBuff);
        addr += realLen;		
       
        if(len > cCrcBuffLenth)
        {
            realLen = cCrcBuffLenth;
        }
        else
        {
            realLen = len;
        }
    }
    CRC_from_calculation_Out  = CRC_from_calculation;
    CRC_from_calculation_Out ^= 0xffffffffL;
    return CRC_from_calculation_Out;
}

const u16 sBmsCRCTalbe[] = {
    0x0000, 0xCC01, 0xD801, 0x1400, 0xF001, 0x3C00, 0x2800, 0xE401, 
    0xA001, 0x6C00, 0x7800, 0xB401, 0x5000, 0x9C01, 0x8801, 0x4400, 
 };

u16 sBMS_CRC16_Calc(u8 *ptr, u16 Count)
{
	u16 wCRC  = 0XFFFF;
    u8  chChar = 0;
    u16 i = 0;
    
    
	for (i = 0; i < Count; i++)
	{
		chChar = *ptr++;
		wCRC = sBmsCRCTalbe[(chChar ^ wCRC) & 0x0F] ^ (wCRC >> 4);
		wCRC = sBmsCRCTalbe[((chChar >> 4) ^ wCRC) & 0x0F] ^ (wCRC >> 4);
	}
	wCRC = (wCRC >> 8)|(wCRC << 8);
	return wCRC;
}


u16 sBMS_CRC32_Calc(u16 crc,u8 *ptr, u16 Count)
{
	u16 wCRC  = crc;
    u8  chChar = 0;
    u16 i = 0;
	
	for (i = 0; i < Count; i++)
	{
		chChar = *ptr++;
		wCRC = sBmsCRCTalbe[(chChar ^ wCRC) & 0x0F] ^ (wCRC >> 4);
		wCRC = sBmsCRCTalbe[((chChar >> 4) ^ wCRC) & 0x0F] ^ (wCRC >> 4);
	}
	//wCRC = (wCRC >> 8)|(wCRC << 8);
	return wCRC;
}


u32 sCalBmsBinDataCrc32(u32 addr, u32 len)
{
	u16 CRC_from_calculation = 0;
    u32 CRC_from_calculation_Out = 0;
    u32 realLen;
	
    if(addr%2 != 0) return 0;
    if(len%2 != 0) return 0;
    
    CRC_from_calculation     = 0xffff;
    if(len > cCrcBuffLenth)
    {
        realLen = cCrcBuffLenth;
    }
    else
    {
        realLen = len;
    }

    while(len > 0)
    {   
        pApp->pFedDog();
        len -= realLen;
        pReadByteData(addr, realLen, crcBuff);
        CRC_from_calculation = sBMS_CRC32_Calc( CRC_from_calculation, crcBuff, realLen);
        addr += realLen;		
       
        if(len > cCrcBuffLenth)
        {
            realLen = cCrcBuffLenth;
        }
        else
        {
            realLen = len;
        }
    }
	//CRC_from_calculation = (CRC_from_calculation >> 8)|(CRC_from_calculation << 8);
    CRC_from_calculation_Out  = CRC_from_calculation;
    //CRC_from_calculation_Out ^= 0xffffffffL;
    return CRC_from_calculation_Out;
}

u32 sGetBmsAppCrc(void)
{
	stBinApp.BmsCrc = sCalBmsBinDataCrc32(pApp->BinFileStartAddr + cAppFieldOffset, stBinApp.size);//ÖØÐÂ¼ÆËã
    return stBinApp.BmsCrc;
}

bool sHeaderCheckIsOk(void)
{    
    sRdBinHeaderSize();
    if(stBinHead.size1 != cHeaderDataLen) return FALSE;
    if(stBinHead.size2 != cHeaderDataLen) return FALSE;

    sRdBinHeaderCrc();
    if(sCalBinDataCrc32(pApp->BinFileStartAddr + cHeaderFieldOffset, cHeaderDataLen-4) != stBinHead.crc) return FALSE;
    
    return TRUE;
}


bool sFileDataRxSizeIsOk(void)
{
    if(RxBinWordSize == preTotal)
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}


bool sAppCheckIsOk(void)
{
    sRdBinAppSize();
    if( stBinApp.size > maxBinFileSize) return FALSE;
    sRdBinAppCrc();
    if(sCalBinDataCrc32(pApp->BinFileStartAddr + cAppFieldOffset, stBinApp.size) != stBinApp.crc) return FALSE  ;
    return TRUE;
}

bool sFileCheckIsOk(void)
{
    if( sFileDataRxSizeIsOk()
        &&sHeaderCheckIsOk()
        && sAppCheckIsOk())
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool sWrFileDataIsOK(void)
{
    u32 temp;
    u16 tempData;
    u32 StartAddr = pApp->BinFileStartAddr;
    u32 EndAddr = pApp->BinFileEndAddr;
    u16 i;
    u32 BinDataOffset;
    u32 Total = mGet32bitValue(stUpgradeData.total);
    temp = 2 * Total;
    if(temp == 0) return FALSE;
    if(temp > (EndAddr - StartAddr)) return FALSE;

    
    if(stUpgradeData.model !=  pApp->ownInfor.model) return FALSE;
    if(stUpgradeData.FirmWareType == pApp->ownInfor.ZonergyFwChip.FwType)
    {
        for(i=0; i<pApp->ownInfor.ZonergyFwChip.chipNum; i++)
        {
            if(stUpgradeData.chip == pApp->ownInfor.ZonergyFwChip.chip[i]) break;
        }
        if(i>=pApp->ownInfor.ZonergyFwChip.chipNum) return FALSE;
    }
    

    if(stUpgradeData.FirmWareType == pApp->ownInfor.PaceBms.FwType)
    {
        for(i=0; i<pApp->ownInfor.PaceBms.chipNum; i++)
        {
            if(stUpgradeData.chip == pApp->ownInfor.PaceBms.chip[i]) 
            {
                break;
            }
         /*   if (stUpgradeData.chip == WEB_UPGRADEDATA_CHIP)
            {
                break;
            }*/
        }
        if(i>=pApp->ownInfor.PaceBms.chipNum) return FALSE;
    }    
    
    if(stUpgradeData.Len > 512) return FALSE;
    BinDataOffset = mGet32bitValue(stUpgradeData.addr);
    temp = 2 * (BinDataOffset) + stUpgradeData.Len;
    if(temp + StartAddr > EndAddr) return FALSE;    
    
    for(u16 i=0; i<stUpgradeData.Len; i++)
    {
        tempData = stUpgradeData.Data[i]>>8;
        tempData |= stUpgradeData.Data[i]<<8;
        stUpgradeData.Data[i] = tempData;
    }
    temp = 2* BinDataOffset;
    pWriteWordDataWithCheck(StartAddr + temp, stUpgradeData.Len, &stUpgradeData.Data[0]);
    if(!pVerifyWordData(StartAddr + temp, stUpgradeData.Len, &stUpgradeData.Data[0]))
    {
        return FALSE;
    }
    if(BinDataOffset != preAddr)
    {
        preAddr = BinDataOffset;
        if(preTotal == 0)
        {
            preTotal = Total;
        }
        else
        {
            if(preTotal != Total)
            { return FALSE;}
            
        }
        RxBinWordSize += stUpgradeData.Len;
    }
    return TRUE;
}

void sMarkTransferFile(s32 time)
{
    dwTransferTime = knlGetTargetTime(time);
}

bool sIsTransferringFile(void)
{
    if(knlIsTimeOut(dwTransferTime))  return FALSE;
    return TRUE;
}

bool sIsRdBinFileAppDataOk(u32 addr, u32 len, u8 **pBuff)
{
    *pBuff = crcBuff;
    if(len <= cCrcBuffLenth)
    {
        pReadByteData(pApp->BinFileStartAddr + cAppFieldOffset + addr, len, crcBuff);        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool sIsRdBinFileHeadDataOk(u32 addr, u32 len, u8 **pBuff)
{
    *pBuff = crcBuff;
    if(len <= cCrcBuffLenth)
    {
        pReadByteData(pApp->BinFileStartAddr + addr, len, crcBuff);        
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}

bool sIsRdBinFileDataOk(u32 addr, u32 len, u8 **pBuff)
{
    return sIsRdBinFileHeadDataOk(addr, len, pBuff);
}



bool IsReqUpgrade(void)
{
    return bReqUpgrade;
}

void sSetReqUpgrade(void)
{
    bReqUpgrade = TRUE;
}

void sResetReqUpgrade(void)
{
    bReqUpgrade = FALSE;
}

void sSetRemoteRunLevel(u16 Level)
{
    stRemoteUpgrade.RunLevel = Level;
}


void sSetRemoteBurnState(u16 state)
{
    stRemoteUpgrade.state = state;
}

void sSetRemoteBurnProgress(u16 progress)
{
    stRemoteUpgrade.progress = progress;
}

void sSetRemoteBurnErrorCode(u16 errorCode)
{
    stRemoteUpgrade.ErrorCode = errorCode;
}

void sInitRemoteBurn(runlevel_t runLevel)
{
    stRemoteUpgrade.BinFile.model = 0;
    stRemoteUpgrade.BinFile.FirmwareType = 0;
    stRemoteUpgrade.BinFile.chip = 0;
    stRemoteUpgrade.BinFile.FirmwareVer.Ver.major = 0;
    stRemoteUpgrade.BinFile.FirmwareVer.Ver.minor = 0;
    stRemoteUpgrade.BinFile.FirmwareVer.Branch = 0;
    stRemoteUpgrade.BinFile.FirmwareVer.Tag = 0;

    stRemoteUpgrade.RunLevel = (u16)runLevel;
    stRemoteUpgrade.state = (u16)cZonFwBurnIdle;
    stRemoteUpgrade.progress = 0;
    stRemoteUpgrade.ErrorCode =  (u16)cZonInvalidErrCode;    
}

void sEnterBootloader(void)
{    
     __disable_irq();
     User_Update = cUserUpdate;
     HAL_NVIC_SystemReset();
     //sJumptoApplication(FLASH_BASE);
}

void sSetUserUpgarde(void)
{
    User_Update = cUserUpdate;
}

void sResetUserUpgrade(void)
{
    User_Update = 0;
}

void sRestart(void)
{    
     __disable_irq();
     sResetUserUpgrade();
     HAL_NVIC_SystemReset();
     //sJumptoApplication(FLASH_BASE);
}

bool sIsUserUpgardeSet(void)
{
    if(User_Update == cUserUpdate) return TRUE;
    return FALSE;
}

u32 sGetPages(u32 Addr)
{
    if(Addr < FLASH_BASE + 128 * FLASH_PAGE_SIZE)
    {
        return (Addr - FLASH_BASE) / FLASH_PAGE_SIZE;
    }
    else
    {
        return 256 + ((Addr - FLASH_BASE - 128 * FLASH_PAGE_SIZE) / FLASH_PAGE_SIZE);
    }    
}


u32 sGetAppStartAddr(void)
{
    return (FLASH_BASE + FLASH_SIZE - pApp->AppAreaSize);
}

u32 sGetAppEndAddr(void)
{
    return (FLASH_BASE + FLASH_SIZE -1);
}

u32 sGetBootVerAddr(void)
{
    if((pApp->BootVerAddr < FLASH_BASE)
        ||(pApp->BootVerAddr > FLASH_BASE + FLASH_SIZE))
    {
        return 0;
    }
    else
    {
        return (pApp->BootVerAddr);
    }    
}

u32 sGetBootStartAddr(void)
{
    return (pApp->BootStartAddr);
}

u32 sGetBootDataStartAddr(void)
{
    return (pApp->BootDataStartAddr);
}

u32 sGetBootDataSize(void)
{
    return (pApp->BootDataSize);
}

void sStrToVer(FirmwareVer_t *pVer, const u8 *pChar, u16 num)
{
    u8 i;
    u8 temp[20];
    u8 Index=0;
    u8 digit=0;
    u16 data = 0;
    u8 kind = 0;
    if(pChar == 0)
    {
        pVer->Ver.data.wHi8 = 0;
        pVer->Ver.data.wLow8 = 0;
        pVer->Branch = 0;
        pVer->Tag = 0;
        return;
    }
    
    for(i=0; i<num; i++)
    {
        if((pChar[i] >= '0')&&(pChar[i] <= '9'))
        {
            temp[Index++] = pChar[i] - '0';
            digit++;
        }
        else if((pChar[i] == '.')||(pChar[i] == '\0'))
        {
            if((digit>0)&&(Index == digit))
            {   data = 0;
                for(u16 multiple=1; digit>0; Index--,digit--)
                {
                    data += temp[Index-1]*multiple;
                    multiple = multiple*10;
                }
                switch(kind)
                {
                    case 0:
                        pVer->Ver.data.wHi8 = (u8)(data);    
                        break;
                    case 1:
                        pVer->Ver.data.wLow8 = (u8)(data);    
                        break;
                    case 2:
                        pVer->Branch = (data);    
                        break;
                    case 3:
                        pVer->Tag = (data);    
                        break;                       
                }
                kind++;
            }
            Index = 0;
            digit = 0;
        }
        else
        {
            Index = 0;
            digit = 0;
        }
        
    }
}

void sGetBootVer(FirmwareVer_t *pVer)
{
    pVer->Ver.all = stBootVer.Ver.all;
    pVer->Branch = stBootVer.Branch;
    pVer->Tag = stBootVer.Tag;
}

u32 sCalChipAppDataCrc32(u32 addr, u32 len)
{
    u32 CRC_from_calculation = 0;
    u32 CRC_from_calculation_Out = 0;
    u32 realLen;
    if(len%4 != 0) return 0;
    
    CRC_from_calculation     = 0xffffffffL;
    if(len > cCrcBuffLenth)
    {
        realLen = cCrcBuffLenth;
    }
    else
    {
        realLen = len;
    }

    while(len > 0)
    {   
        pApp->pFedDog();
        len -= realLen;       
        for(u16 i=0; i<realLen; i++)
        {
            crcBuff[i] = *(u8*)(addr+i);
        }
        
        CRC_from_calculation = CalculateCrc32( CRC_from_calculation, realLen, crcBuff);
        addr += realLen;		
       
        if(len > cCrcBuffLenth)
        {
            realLen = cCrcBuffLenth;
        }
        else
        {
            realLen = len;
        }
    }
    CRC_from_calculation_Out  = CRC_from_calculation;
    CRC_from_calculation_Out ^= 0xffffffffL;
    return CRC_from_calculation_Out;
}

bool sIsChipAppCheckOk(void)
{
    sRdBinAppSize();
    if( stBinApp.size > maxBinFileSize) return FALSE;
    sRdBinAppCrc();
    if(sCalChipAppDataCrc32(sGetAppStartAddr(), stBinApp.size) != stBinApp.crc) return FALSE  ;
    return TRUE;
}

void sSetAppOkFlag(void)
{
    u32 endAddr = sGetAppEndAddr();
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, endAddr-7, cAppOk);
    HAL_FLASH_Lock();    
}

void sClrAppOkFlag(void)
{
    u32 endAddr = sGetAppEndAddr();
    HAL_FLASH_Unlock();
    HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, endAddr-7, 0x00);
    HAL_FLASH_Lock();    
}

bool IsAppOK(void)
{
    u64 AppOkFlag;
    u32 endAddr = sGetAppEndAddr();
    AppOkFlag = *(u64*)(endAddr-7);
    if(AppOkFlag == cAppOk)
    {
        return TRUE;
    }
    return FALSE;
}



bool IsCheckAppOkFlagClrOk(void)
{
    #ifdef CHECK_APP_OK
        return TRUE;
    #elif NO_CHECK_APP_OK
        return TRUE;
    #else
        if(*(u64*)(NeedCheckAppOk) == 0x00) 
        {
        	return TRUE;
        }
        else
        {
        	return FALSE;
        }
    #endif
}

void sClrCheckAppOkFlag(void)
{
    #ifdef CHECK_APP_OK

    #elif NO_CHECK_APP_OK

    #else
        u32 Addr = (u32)(NeedCheckAppOk);
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr, 0x00);
        HAL_FLASH_Lock();
    #endif  
}

bool sIsEraseAppAreaOk(void)
{
    u32 startAddr = sGetAppStartAddr();
    u32 startPage;
    u32 NbPages;
    FLASH_EraseInitTypeDef stEraseInit;
    u32 PageError;
    bool result = TRUE;

    sClrAppOkFlag();
    if(IsAppOK())
    {
        return FALSE;
    }
    
    startAddr = sGetAppStartAddr();
    if(startAddr < FLASH_BASE + 128 * FLASH_PAGE_SIZE)
    {
        //bank1     
        startPage = sGetPages(startAddr);
        NbPages = sGetPages(FLASH_BASE + 128 * FLASH_PAGE_SIZE -1) - startPage +1;
        stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        stEraseInit.Banks = FLASH_BANK_1;
        stEraseInit.Page = startPage;
        stEraseInit.NbPages = NbPages;
        //__disable_irq();
        HAL_FLASH_Unlock();
        if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
        {
            result = FALSE;
        }
        HAL_FLASH_Lock();//FLASH½âËø
        //__enable_irq();
        startAddr = FLASH_BASE + 128 * FLASH_PAGE_SIZE;
    }
    if(result)
    {
        startPage = sGetPages(startAddr);
        NbPages = sGetPages(sGetAppEndAddr()) - startPage +1;
        stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        stEraseInit.Banks = FLASH_BANK_2;
        stEraseInit.Page = startPage;
        stEraseInit.NbPages = NbPages;
        //__disable_irq();
        HAL_FLASH_Unlock();
        if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
        {
            result = FALSE;
        }
        HAL_FLASH_Lock();//FLASH½âËø
        //__enable_irq();
    }
    return result;
}

bool sIsErasePageOk(u32 addr)
{
    u32 validAddr;
    validAddr = addr;
    u32 startPage;
    FLASH_EraseInitTypeDef stEraseInit;
    u32 PageError;
    bool result = TRUE;
    
    startPage = sGetPages(validAddr);
    stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
    if(validAddr < FLASH_BASE + 128 * FLASH_PAGE_SIZE)
    {
        stEraseInit.Banks = FLASH_BANK_1;
    }
    else
    {
        stEraseInit.Banks = FLASH_BANK_2;
    }
    stEraseInit.Page = startPage;
    stEraseInit.NbPages = 1;
    HAL_FLASH_Unlock();
    if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
    {
        result = FALSE;
    }
    HAL_FLASH_Lock();//FLASH½âËø
   return result;
}

bool sIsEraseAppPageOk(u32 addr)
{
    return sIsErasePageOk(sGetAppStartAddr() + addr);    
}

bool sIsEraseBootPageOk(u32 addr)
{
    return sIsErasePageOk(sGetBootStartAddr() + addr);
}

bool sIsWrAppAreaOk(u32 addr, u32 len)
{ 
    u64 temp;
    u8 tempByte;
    u32 Addr;
    bool result = TRUE;
    if(len > cCrcBuffLenth) return FALSE;
    if(len % 4 != 0) return FALSE;
    Addr = sGetAppStartAddr() + addr; 

    if(len % 8 != 0)
    {
        for(u16 i=0; i<cCrcBuffLenth; i++)
        {
            crcBuff[i] = 0xff;
        }
    }

    pReadByteData(pApp->BinFileStartAddr + cAppFieldOffset + addr, len, crcBuff);
    //__disable_irq();
    HAL_FLASH_Unlock();
    for(u16 i=0; i<len; i=i+8)
    {    
        temp = *((u64*)(&crcBuff[i]));
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr+i, temp) != HAL_OK)
        {
            result = FALSE;
            break;
        }
    }
    HAL_FLASH_Lock();
    for(u16 i=0; i<len; i++)
    {
        tempByte = *((u8*)(Addr+i));
        if(crcBuff[i] != tempByte)
        {
            result = FALSE;
            break;
        }
    }    
    //__enable_irq();
    return result;       
}

__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}


bool sJumptoApplication(u32 AppAddr)
{
   bool ret = FALSE;
   void(*ptr)(void);
   if((*(__IO u32*)AppAddr & 0x2ffe0000) == 0x20000000)
   {
       //×°ÔØmsp
       MSR_MSP((*(u32*)AppAddr));
       //Ìø×ª
       ptr = (void(*)(void))(*(__IO u32*)(AppAddr+4));

	   ptr();
   }
   return ret;        
}

bool sIsNeedCheckAppOk(void)
{
    #ifdef CHECK_APP_OK   
        return TRUE;
    #elif NO_CHECK_APP_OK
        return TRUE;
    #else
        if(*(u64*)(NeedCheckAppOk) == cNoCheckAppOKFlag) 
        {
            return FALSE;
        }
        else
        {
            return TRUE;
        }
    #endif
}

/*******************************Burn boot*******************************/
void sClrBurnBootFlag(void)
{
    #ifdef BURN_BOOT
        u32 Addr = (u32)(&BurnBoot);
        HAL_FLASH_Unlock();
        HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, Addr, 0x00);
        HAL_FLASH_Lock();
    #elif NO_BURN_BOOT

    #else
        
    #endif  
}

bool sIsNeedBurnBoot(void)
{
    #ifdef BURN_BOOT     
        u32 Addr = (u32)(&BurnBoot);
        if(*((u64*)Addr) == cNeedBurnBootFlag) 
        {
            return TRUE;
        }
        else
        {
            return FALSE;
        }
    #elif NO_BURN_BOOT
        return FALSE;
    #else
        return FALSE;
    #endif
}

bool sIsHasBoot(void)
{
    if((sGetBootStartAddr() < FLASH_BASE)||(sGetBootStartAddr()>sGetAppEndAddr())) return FALSE;
    if((sGetBootDataStartAddr() < FLASH_BASE)||(sGetBootDataStartAddr()>sGetAppEndAddr())) return FALSE;    
    if(sGetBootDataSize() <100) return FALSE;
    return TRUE;
}


bool sIsBurningBoot(void)
{
    return bIsBurningBoot;
}

bool sIsEraseBootAreaOk(void)
{
    u32 startAddr = sGetBootStartAddr();
    u32 endAddr = startAddr + sGetBootDataSize() - 1;
    u32 startPage;
    u32 NbPages;
    FLASH_EraseInitTypeDef stEraseInit;
    u32 PageError;
    bool result = TRUE;
    
    if((startAddr < FLASH_BASE + 128 * FLASH_PAGE_SIZE) && (endAddr >= FLASH_BASE + 128 * FLASH_PAGE_SIZE))
    {
        //bank1     
        startPage = sGetPages(startAddr);
        NbPages = sGetPages(FLASH_BASE + 128 * FLASH_PAGE_SIZE - 1) - startPage +1;
        stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        stEraseInit.Banks = FLASH_BANK_1;
        stEraseInit.Page = startPage;
        stEraseInit.NbPages = NbPages;
        //__disable_irq();
        HAL_FLASH_Unlock();
        if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
        {
            result = FALSE;
        }
        HAL_FLASH_Lock();//FLASH½âËø
        //__enable_irq();
        
        //bank2
        startPage = sGetPages(FLASH_BASE + 128 * FLASH_PAGE_SIZE);
        NbPages = sGetPages(endAddr) - startPage +1;
        stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        stEraseInit.Banks = FLASH_BANK_2;
        stEraseInit.Page = startPage;
        stEraseInit.NbPages = NbPages;
        //__disable_irq();
        HAL_FLASH_Unlock();
        if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
        {
            result = FALSE;
        }
        HAL_FLASH_Lock();//FLASH½âËø
        //__enable_irq();
    }
    else
    { 
        startPage = sGetPages(startAddr);
        NbPages = sGetPages(endAddr) - startPage +1;
        stEraseInit.TypeErase = FLASH_TYPEERASE_PAGES;
        if(endAddr < FLASH_BASE + 128 * FLASH_PAGE_SIZE)
        {
            stEraseInit.Banks = FLASH_BANK_1;
        }
        else
        {
            stEraseInit.Banks = FLASH_BANK_2;
        }
        stEraseInit.Page = startPage;
        stEraseInit.NbPages = NbPages;
        //__disable_irq();
        HAL_FLASH_Unlock();
        if(HAL_FLASHEx_Erase(&stEraseInit, &PageError) != HAL_OK)//²Á³ýFLASH
        {
            result = FALSE;
        }
        HAL_FLASH_Lock();//FLASH½âËø
        //__enable_irq();
    }
    return result;
}


bool sIsWrBootAreaOk(void)
{     
    u64 temp;
    u32 i,j;
    u32 BootStartAddr;
    u32 BootDataAddr;
    u32 BootDataSize;

    u8 Data[8];
    bool result = TRUE;
    BootStartAddr = sGetBootStartAddr();
    BootDataAddr = sGetBootDataStartAddr();
    BootDataSize = sGetBootDataSize();
    
    //__disable_irq();    
    HAL_FLASH_Unlock();
    for(i=0; i<BootDataSize; i=i+8)
    {    
        for(j=0;j<8;j++)
        {
            Data[j] = *(u8*)(BootDataAddr+i+j);
        }
        
        if(i >= BootDataSize-8)
        {
            for(j=0;j<8;j++)
            {
                Data[j] = 0xFF;
            }
            for(j=0;j<BootDataSize-i;j++)
            {
                Data[j] = *(u8*)(BootDataAddr+i+j);
            }
        }
        temp = *((u64*)(&Data[0]));        
        if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_DOUBLEWORD, BootStartAddr+i, temp) != HAL_OK)
        {
            result = FALSE;
            break;
        }        
    }
    HAL_FLASH_Lock();
    //__enable_irq();
    
    for(i=0; i<BootDataSize; i++)
    {
        if(*(u8*)(BootDataAddr+i) != *(u8*)(BootStartAddr+i))
        {
            result = FALSE;
            break;
        }
    }    
    return result;       
}


void sBurnBoot(void)
{       
    sSetRemoteBurnState((u16)cZonFwBootBurnIdle);
    sSetRemoteBurnErrorCode((u16)cZonInvalidErrCode);
    bIsBurningBoot = TRUE;

    sSetRemoteBurnState((u16)cZonFwBootErasing);
    
    if(!sIsEraseBootAreaOk())
    {
        sSetRemoteBurnState((u16)cZonFwBootBurnFail);
        sSetRemoteBurnErrorCode((u16)cZonFwBootEraseFail);
        bIsBurningBoot = FALSE;
        return;
    }
    
    sSetRemoteBurnState((u16)cZonFwBootWriting);
    if(!sIsWrBootAreaOk())
    {
        sSetRemoteBurnState((u16)cZonFwBootBurnFail);
        sSetRemoteBurnErrorCode((u16)cZonFwBootWriteFail);
        bIsBurningBoot = FALSE;
        return;
    }
    
    sReadBootVer(&stBootVer);
    sSetRemoteBurnState((u16)cZonFwBootBurnPass);
    bIsBurningBoot = FALSE;
    sClrBurnBootFlag();
}













