#ifndef STRING_H
#define STRING_H
#include "typedef.h"

typedef enum{
    eCapital,
	elowercase,
}LetterType_t;



s16 sStrCopy(char *pSor,char *pDim);
s16	swGetStrLen(char *bStr);
void ToxAsc(s16 wData,char *bStr);
void ToxxAsc(s16 wData,char *bStr);
void ToxxxAsc(s16 wData,char *bStr);
void ToxxxxAsc(s16 wData,char *bStr);
s16 HexToAsc(long wData,char *bStr);
s16 HexToStr(long wData,char *bStr,int wDot);
s16 sStrcmp(char* p1, char* p2);
void ToHexAsc(u8 data, char *bStr, LetterType_t type);
void* my_memmove(void* dest, const void* src, u8 num);


#endif
