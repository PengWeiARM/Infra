#include "uc1698.h"
#include "Draw.h"
#include "font.h"
void XLine(int x1,int x2,int y)
{
	int Xs,Xe;
	int i;
	if(x1==x2)return;
	if(y<Y_Start)y=Y_Start;if(y>Y_End)y=Y_End;
    y--;
	if(x1<X_Start)x1=X_Start;if(x1>X_End)x1=X_End;
    x1--;
	if(x2<X_Start)x2=X_Start;if(x2>X_End)x2=X_End;
    x2--;
	Xs=x1/16;
	Xe=x2/16;
	for(i=Xs+1;i<Xe;i++)
	{
		DispBuf[y][i] = 0xffff;			   
	}
	if(Xs!=Xe)
	{
			DispBuf[y][Xs] |= (0xffff >> (x1 - Xs*16));
		if(x2%15!=0)
			DispBuf[y][Xe] |= (0xffff << (16+Xe*16-x2-1));
	}
	else
	{
		DispBuf[y][Xs] |= ((0xffff >> (x1 - Xs*16))&(0xffff << (16+Xe*16-x2-1)));
	}
}

void YLine(int x,int y1,int y2)
{
	int i;
	int Xs;
	if(y1==y2)return;

	if(y1>Y_End)return; 	//y1=Y_End;
	if(y2>Y_End)return; 	//y2=Y_End;

	if(y1>y2){Xs=y1;y1=y2;y2=Xs;}	

	if(x<X_Start)return; 		//x=X_Start;
	if(x>X_End)return; 		//x=X_End;
	x--;
	if(y1<Y_Start)y1=Y_Start; 		//y1=Y_Start;
	y1--;
	if(y2<Y_Start)y2=Y_Start; 		//y2=Y_Start;
    y2--;
	Xs=x/16;
	for(i=y1;i<=y2;i++)
	{
		DispBuf[i][Xs]|=(0x8000>>(x-Xs*16));
	}
}

void PutPixel(int x,int y)
{
	int temp;
	if(y<Y_Start)return;if(y>Y_End)return;
    y--;
	if(x<X_Start)return;if(x>X_End)return;
    x--;
	temp = x/16;
	DispBuf[y][temp]|=(0x8000>>(x-temp*16));
}

void Rectangle(int x1,int y1,int x2,int y2)
{
	XLine(x1,x2,y1);
	XLine(x1,x2,y2);
	YLine(x1,y1,y2);
	YLine(x2,y1,y2);
}

void RectangleFill(int x1,int y1,int x2,int y2)
{
	if(y1<y2)
	{
		while(y1!=y2)
		{
			XLine(x1,x2,y1++);
		}
	}
	else if(y1>y2)
	{
		while(y1!=y2)
		{
			XLine(x1,x2,y1--);
		}	
	}
}

void ClearArea(int x1,int y1,int x2,int y2)
{
	int y;
	int Xs,Xe;
	int i;
	if(x1==x2)return;
	if(y1<Y_Start)y1=Y_Start;if(y1>Y_End)y1=Y_End;
	if(y2<Y_Start)y2=Y_Start;if(y2>Y_End)y2=Y_End;
	if(x1<X_Start)x1=X_Start;if(x1>X_End)x1=X_End;
	if(x2<X_Start)x2=X_Start;if(x2>X_End)x2=X_End;
    y1--;
    y2--;
    x1--;
    x2--;
	for(y=y1;y<=y2;y++)
	{
		Xs=x1/16;
		Xe=x2/16;
		for(i=Xs+1;i<Xe;i++)
		{
			DispBuf[y][i] = 0;
		}
		if(Xs!=Xe)
		{
				DispBuf[y][Xs] &= ~(0xffff >> (x1 - Xs*16));
			if(x2%15!=0)
				DispBuf[y][Xe] &= ~(0xffff << (16+Xe*16-x2-1));
		}
		else
		{
			DispBuf[y][Xs] &= ~((0xffff >> (x1 - Xs*16))&(0xffff << (16+Xe*16-x2-1)));
		}
	}
}

void PutPic(int xStart,int yStart,int wide,int high,const char *pData,bool wInv)
{
	int x,y,bit;
	int yEnd,xEnd;
	int DispAdr;
	u8	DispData;

	if(xStart<X_Start)xStart=X_Start;
    xStart--;
    xEnd = xStart+wide;
    if(xEnd>X_End)xEnd=X_End;

    if(yStart<Y_Start)yStart=Y_Start;
    yStart--;
    yEnd = yStart+high;
    if(yEnd>Y_End)yEnd=Y_End;

	for(y=yStart;y<yEnd;y++)
	{
		for(x=xStart;x<xEnd;)
		{
			DispData = *pData++;
			for(bit=0;bit<8;bit++)
			{
				DispAdr = x/16;
				if(wInv)
				{
					if(DispData&0x80)
					{
						DispBuf[y][DispAdr]&=~(0x8000>>(x-DispAdr*16));
					}
					else
					{
						DispBuf[y][DispAdr]|=(0x8000>>(x-DispAdr*16));						
					}
				}
				else
				{
					 if(DispData&0x80)
					{
						DispBuf[y][DispAdr]|=(0x8000>>(x-DispAdr*16));
					}
					else
					{
						DispBuf[y][DispAdr]&=~(0x8000>>(x-DispAdr*16));
					}
				}
				DispData<<=1;
				x++;
			}
		}
	}
}

void PutRoll(int Postive,int ItemNum)
{
	int wRowHigh;	
	if(Postive>=ItemNum)
	{
	   return;
	}
 	wRowHigh = 	Row_High*10/ItemNum;
	ClearArea(Row_X-3,Row_Y_Start,Row_X+3,Row_Y_End);
    Rectangle(Row_X-2,Row_Y_Start+(Postive*wRowHigh)/10,Row_X+2,Row_Y_Start+(Postive+1)*wRowHigh/10);
	YLine(Row_X ,Row_Y_Start ,Row_Y_Start+Postive*wRowHigh/10 );
	YLine(Row_X ,Row_Y_Start+(Postive+1)*wRowHigh/10 ,Row_Y_End );
}


void PutHz(int x,int y ,char *pData,int wLen,bool wInv)
{
/*	while(wLen--)
	{
		PutPic(x,y,16,16,Hz[*pData],wInv);
		pData++;
		x+=16;
	}*/
}

void Put2232Asc(int x,int y,char *pData,int wLen,bool wInv)
{
	while(wLen--)
	{
		if(y+32>Y_End) return;
        if(x+24>X_End)  return;
		if((*pData<='~')&&(*pData>=' '))
		{
			PutPic(x,y,24,32,ASC2232[*pData-' '],wInv);
		}
		else if(*pData==0xC3)
		{
			pData++;
			PutPic(x,y,24,32,ASC2232[*pData-' '],wInv);
			wLen--;
		}
		else
		{PutPic(x,y,24,32,ASC2232[0],FALSE);}
		pData++;
		x+=22;
	}
}

void Put0816Asc(int x,int y,char *pData,int wLen,bool wInv)
{
	while(wLen--)
	{
		if(y+8>Y_End) return;
        if(x+8>X_End)  return;
		if((*pData<='~')&&(*pData>=' '))
		{
			PutPic(x,y,8,16,AscII[*pData-' '],wInv);
		}
		else if(*pData==0xC3)
		{
			pData++;
			PutPic(x,y,8,16,AscII[*pData-' '],wInv);
			wLen--;
		}
		else
		{PutPic(x,y,8,16,AscII[0],wInv);}
		pData++;
		x+=8;
	}
}

void Put0612Asc(int x,int y,char *pData,int wLen,int Space)
{
	if(x<0)x=0;
	while(wLen--)
	{
		if(y+8>Y_End) return;
        if(x+6>X_End)  return;
		if((*pData<='~')&&(*pData>=' '))
		{
			PutPic(x,y,6,12,ASC0612[*pData-' '],FALSE);
		}
		else if(*pData==0xC3)
		{
			pData++;
			PutPic(x,y,6,12,ASC0612[*pData-' '],FALSE);
			wLen--;
		}
		else
		{PutPic(x,y,6,12,ASC0612[0],FALSE);}
		pData++;
		x+=(6+Space);
	}
}


s32 wPow(s32 data,u16 p)
{
   while(p--)
   {
		data=data*10;
   }
   return data;
}

void InverArea(int x1,int y1,int x2,int y2)
{
	int y;
	int Xs,Xe;
	int i;
	if(x1==x2)return;
	if(y1<Y_Start)y1=Y_Start;if(y1>Y_End)y1=Y_End;
	if(y2<Y_Start)y2=Y_Start;if(y2>Y_End)y2=Y_End;
	if(x1<X_Start)x1=X_Start;if(x1>X_End)x1=X_End;
	if(x2<X_Start)x2=X_Start;if(x2>X_End)x2=X_End;
    y1--;
    y2--;
    x1--;
    x2--;
	for(y=y1;y<=y2;y++)
	{
		Xs=x1/16;
		Xe=x2/16;
		for(i=Xs+1;i<Xe;i++)
		{
			DispBuf[y][i] ^= 0xffff;
		}
		if(Xs!=Xe)
		{
				DispBuf[y][Xs] ^= (0xffff >> (x1 - Xs*16));
			if(x2%15!=0)
				DispBuf[y][Xe] ^= (0xffff << (16+Xe*16-x2-1));
		}
		else
		{
			DispBuf[y][Xs] ^= ((0xffff >> (x1 - Xs*16))&(0xffff << (16+Xe*16-x2-1)));
		}
	}
}

