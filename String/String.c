#include "string.h"

s16 sStrCopy(char *pSor,char *pDim)
{
	int wLen=0;
	while(*pSor!=0)
	{
		*pDim++ = *pSor++;
		wLen++;
		if(wLen>30)
		{
			*pDim = 0;
		 	return 0;
		}
	}
	*pDim = 0;
	return wLen;
}

int	swGetStrLen(char *bStr)
{
	int wLen=0;
	while(*bStr++!=0)
	{
		wLen++;
		if(wLen>80)
		{
		 	return 0;
		}
	}
	return wLen;

}

void ToxAsc(int wData,char *bStr)
{
	*bStr++ = (wData)+'0';
	*bStr++ = 0;
}


void ToxxAsc(int wData,char *bStr)
{
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}

void ToxxxAsc(int wData,char *bStr)
{
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}
void ToxxxxAsc(int wData,char *bStr)
{
	*bStr++ = (wData/1000)+'0';
	wData%=1000;
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}

void ToxxxxxAsc(long wData,char *bStr)
{
	*bStr++ = (wData/10000)+'0';
	wData%=10000;
	*bStr++ = (wData/1000)+'0';
	wData%=1000;
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}

void ToxxxxxxAsc(long wData,char *bStr)
{
	*bStr++ = (wData/100000l)+'0';
	wData%=100000l;
	*bStr++ = (wData/10000)+'0';
	wData%=10000;
	*bStr++ = (wData/1000)+'0';
	wData%=1000;
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}
void ToxxxxxxxAsc(long wData,char *bStr)
{
	*bStr++ = (wData/1000000l)+'0';
	wData%=1000000l;
	*bStr++ = (wData/100000l)+'0';
	wData%=100000l;
	*bStr++ = (wData/10000)+'0';
	wData%=10000;
	*bStr++ = (wData/1000)+'0';
	wData%=1000;
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}
int HexToAsc(long wData,char *bStr)
{
	int wSign=0;
	if(wData<0)
	{
		wSign = 1;
		wData =-wData;
		*bStr++ ='-';
	}
	if(wData>=1000000)
	{
		ToxxxxxxxAsc(wData,bStr);
		return 7+wSign;
	}
	else if(wData>=100000)
	{
		ToxxxxxxAsc(wData,bStr);
		return 6+wSign;
	}
	else if(wData>=10000)
	{
		ToxxxxxAsc(wData,bStr);
		return 5+wSign;
	}
	else if(wData>=1000)
	{
		ToxxxxAsc(wData,bStr);
		return 4+wSign;
	}
	else if(wData>=100)
	{
		ToxxxAsc(wData,bStr);
		return 3+wSign;
	}
	else if(wData>=10)
	{
		ToxxAsc(wData,bStr);
		return 2+wSign;
	}
	else
		*bStr++=wData+'0';
	return 1+wSign;
}

int HexToStr(long wData,char *bStr,int wDot)
{  
   int bitIndex;
   int bit;
   int wLen;
   int wSign;	 
   char *pDataTemp;
   char *pStrTemp;
   char DataTemp[10];
   pStrTemp = bStr;
   pDataTemp = bStr;
   bitIndex = HexToAsc(wData,bStr);
   if(wDot == 0)
   {
	 	return bitIndex;
   }
   
   if(wData<0)
   {
		wSign = 1;
	}
	else
	{
		wSign = 0;
	}

   	
   if((bitIndex-wSign)>wDot)
   {  
       bit = bitIndex;
   	   pDataTemp[bitIndex+1] = 0;
	   while(wDot--)
	   {
	   		pDataTemp[bitIndex] = pDataTemp[bitIndex-1];
			bitIndex--;
	   }
	   pDataTemp[bitIndex] = '.';
	   return bit+1;
   }
   else	
   {	
   	   
       for(wLen=0;wLen<10;wLen++)
	   {
		  DataTemp[wLen] = '0';
	   }
	   if(wData<0)
	   {
	   		DataTemp[0] ='-';
			DataTemp[2] ='.';
	   }
	   else
	   {
		 	DataTemp[1] ='.';
	   }
	
	   wLen = bitIndex - wSign;
	   bitIndex = wDot - bitIndex + 2 + wSign*2;
	   bit = wSign;
	   while(wLen--)
	   {
	   	 DataTemp[bitIndex++] =	pStrTemp[bit++];
	   }	

	   DataTemp[wDot+2+ wSign] = 0;
       for(wLen=0;wLen<10;wLen++)
	   {
		  *pDataTemp++ = DataTemp[wLen];
	   }
	   	return wDot+2+ wSign;
   }
}	

#define N 30
int sStrcmp(char* p1, char* p2) {
	for (int i = 0; i < N; ++i) {
		if (p1[i] != p2[i]) {
			return p1[i] - p2[i];
		}
		else if (p1[i] == '\0' && p2[i] == '\0') {
			return 0;
		}
	}
	return 0;
}


