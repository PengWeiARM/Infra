#include "string.h"
#include <assert.h>
#include "stdio.h"
s16 sStrCopy(char *pSor,char *pDim)
{
	s16 wLen=0;
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

s16	swGetStrLen(char *bStr)
{
	s16 wLen=0;
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

void ToxAsc(s16 wData,char *bStr)
{
	*bStr++ = (wData)+'0';
	*bStr++ = 0;
}


void ToxxAsc(s16 wData,char *bStr)
{
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}

void ToxxxAsc(s16 wData,char *bStr)
{
	*bStr++ = (wData/100)+'0';
	wData%=100;
	*bStr++ = (wData/10)+'0';
	*bStr++ = (wData%10)+'0';
	*bStr++ = 0;
}
void ToxxxxAsc(s16 wData,char *bStr)
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

void ToHexAsc(u8 data, char *bStr, LetterType_t Type)
{
    if(data < 10)
    {
        *bStr = (data)+'0';
    }
    else if(data < 16)
    {
        if(Type == eCapital)
        {
            *bStr = (data-10)+'A';
        }
        else if(Type == elowercase)
        {
            *bStr = (data-10)+'a';
        }
        else
        {
            *bStr = 0;
        }
    }
    else
    {
        *bStr = 0;
    }
}

s16 HexToAsc(long wData,char *bStr)
{
	s16 wSign=0;
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

s16 HexToStr(long wData,char *bStr,int wDot)
{  
   s16 bitIndex;
   s16 bit;
   s16 wLen;
   s16 wSign;	 
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
s16 sStrcmp(char* p1, char* p2) {
	for (s16 i = 0; i < N; ++i) {
		if (p1[i] != p2[i]) {
			return p1[i] - p2[i];
		}
		else if (p1[i] == '\0' && p2[i] == '\0') {
			return 0;
		}
	}
	return 0;
}

//模拟实现my_memmove()函数
 
void* my_memmove(void* destination, const void* source, int num)
{
	assert(destination);        //防止源头或目的地指针为NULL
	assert(source);
	void* ret = destination;
	if (source < destination)   //内存中数据的存储是由低地址到高地址的
	{
		//从后向前拷贝
		while (num--)   //以num为20为例,在num进入while循环之后,就立刻--,变成19了
		{
			*((char*)destination + num ) = *((char*)source + num );
			//这时source+num刚好指向的是源头内存块的最后一个字节
		}
	}
	else
	{
		while (num--)
		{
			//从前向后拷贝
			*((char*)destination) = *((char*)source);
			++((char*)destination);        
			++((char*)source);
			//这里使用后置++的话一定要给(char*)destination整体带上括号
			//否则后置++的优先级比(char*)的强制类型转换的优先级高,
			//导致指针类型还是void*时就进行++操作,这是在C标准中是不允许的
		}
	}
	return ret;
}


//memset函数
void* my_memset(void* ptr, int value, int num)
{
	//参数有效性检查
	assert(ptr != NULL);
	char* tptr = (char*)ptr;
	//初始化内存
	while (num-- != 0)
	{
		*tptr++ = value;
	}
	return ptr;
}





