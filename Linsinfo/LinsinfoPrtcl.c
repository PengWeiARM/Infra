#include "LinsinfoPrtcl.h"



/**
 * @brief 
 * @param pData Struct of Linsinfo Protocol
 * @return 0:No Arc;  1:Arc in Region A;  2:Arc in Region B;  3:Arc in Region C;
 */
unsigned char uCheckArc(LinsinfoCmd_t *pData)
{
    if ((pData->uwHeader == 0x555A) && (pData->uCmd == eLinsinfo_CheckArc))
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
    ProBarLinsinfo.uwHeader = 0x555A;
    ProBarLinsinfo.uwLength = 0x0001;
    ProBarLinsinfo.uCmd = 0x10;

    return ProBarLinsinfo;
}

/**
 * @brief 
 * @return 
 */
LinsinfoDataI_t PackTransCurr1Ch(unsigned char uAddr, _Bool Enable,
                            unsigned char uInte,unsigned char uDeci)
{
    LinsinfoDataI_t ProBarLinsinfo;
    ProBarLinsinfo.uAddr = uAddr;
    ProBarLinsinfo.uwHeader = 0x666A;
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








