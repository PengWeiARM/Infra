#ifndef LINSINFOPRTCL_H
#define LINSINFOPRTCL_H

enum LinsinfoCmd
{
    eLinsinfo_NULL,
    eLinsinfo_GetInfo           = 0x01,
    eLinsinfo_StartLearn        = 0x04,
    eLinsinfo_StopLearn         = 0x05,
    eLinsinfo_ClearLearnData    = 0x07,
    eLinsinfo_CheckLearnState   = 0x0A,
    eLinsinfoCmd_testArcFunc       = 0x0D,
    eLinsinfo_CheckArc          = 0x10,
    eLinsinfo_GetCurr           = 0x13,
    eLinsinfo_SetThresholdTimes = 0x17,
    eLinsinfo_GetThresholdTimes = 0x18,
};

typedef struct
{
    unsigned char uAddr;         //uint8_t
    unsigned char uHeader_8Hi; 
    unsigned char uHeader_8Lo;
    char Channal;                //int8_t   bit0:6 Num; bit7(sign) Enable;
    unsigned char uData1Inte;    //uint8_t
    unsigned char uData1Deci;    //uint8_t
} LinsinfoDataI_t;


typedef struct
{
    unsigned char uAddr;        //uint8_t
    unsigned char uHeader_8Hi; 
    unsigned char uHeader_8Lo;
    unsigned char uLength_8Hi;
    unsigned char uLength_8Lo;  
    unsigned char uCmd;         //uint8_t
    unsigned char uData[60];    //uint8_t  60byte
} LinsinfoCmd_t;


//extern unsigned char (*pFunction)(LinsinfoCmd_t *pData);
//void sProLinsinfo(LinsinfoCmd_t *pData);

unsigned char uCheckArc(LinsinfoCmd_t *pData);
LinsinfoCmd_t PackQueryArcState(unsigned char uAddr);
LinsinfoDataI_t PackTransCurr1Ch(unsigned char uAddr,_Bool Enable,unsigned char uInte,unsigned char uDeci);
LinsinfoCmd_t ResetAFCI(unsigned char uAddr);



#endif
