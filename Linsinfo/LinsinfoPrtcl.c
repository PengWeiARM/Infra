#include "LinsinfoPrtcl.h"



/**
 * @brief 
 * @param pData Struct of Linsinfo Protocol Rx
 * @return 0:No Arc;  1:Arc in Region A;  2:Arc in Region B;  3:Arc in Region C;
 */
unsigned char uCheckArc(LinsinfoCmd_t *pData)
{
    if ((pData->uHeader_8Hi == 0x55) && (pData->uHeader_8Lo == 0x5A) 
        && (pData->uCmd == eLinsinfo_CheckArc))
    {
        return pData->uData[0];
    }
    else{}
    return 0;
}

/**
 * @brief 
 * @return 
 */
LinsinfoCmd_t PackQueryArcState(unsigned char uAddr)
{
    LinsinfoCmd_t ProBarLinsinfo;
    ProBarLinsinfo.uAddr = uAddr;
    ProBarLinsinfo.uHeader_8Hi = 0x55;
    ProBarLinsinfo.uHeader_8Lo = 0x5A;
    ProBarLinsinfo.uLength_8Hi = 0x00;
    ProBarLinsinfo.uLength_8Lo = 0x01;
    ProBarLinsinfo.uCmd = 0x10;

    return ProBarLinsinfo;
}

/**
 * @brief 
 * @param uAddr 
 * @param Enable 1 bit enable Arc Alarm
 * @param uInte Inieger of Current
 * @param uDeci Decimals of Current
 * @return 
 */
LinsinfoDataI_t PackTransCurr1Ch(unsigned char uAddr, _Bool Enable,
                                unsigned char uInte,unsigned char uDeci)
{
    LinsinfoDataI_t ProBarLinsinfo;
    ProBarLinsinfo.uAddr = uAddr;
    ProBarLinsinfo.uHeader_8Hi = 0x66;
    ProBarLinsinfo.uHeader_8Lo = 0x6A;
    if(Enable)
    {
        ProBarLinsinfo.Channal = 0x01;
    }
    else
    {
        ProBarLinsinfo.Channal = 0x81;
    }
    ProBarLinsinfo.uData1Inte = uInte;
    ProBarLinsinfo.uData1Deci = uDeci;

    return ProBarLinsinfo;
}

/**
 * @brief 
 * @param uAddr 
 * @return 
 */
LinsinfoCmd_t ResetAFCI(unsigned char uAddr)
{
    LinsinfoCmd_t ProBarLinsinfo;
    ProBarLinsinfo.uAddr = uAddr;
    ProBarLinsinfo.uHeader_8Hi = 0x55;
    ProBarLinsinfo.uHeader_8Lo = 0x5A;
    ProBarLinsinfo.uLength_8Hi = 0x00;
    ProBarLinsinfo.uLength_8Lo = 0x01;
    ProBarLinsinfo.uCmd = 0xFF;

    return ProBarLinsinfo;
}





