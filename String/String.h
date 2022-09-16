#ifndef STRING_H
#define STRING_H
#include "typedef.h"

s16 sStrCopy(char *pSor,char *pDim);
int	swGetStrLen(char *bStr);
void ToxAsc(int wData,char *bStr);
void ToxxAsc(int wData,char *bStr);
void ToxxxAsc(int wData,char *bStr);
void ToxxxxAsc(int wData,char *bStr);

int HexToStr(long wData,char *bStr,int wDot);
int sStrcmp(char* p1, char* p2);

#endif
