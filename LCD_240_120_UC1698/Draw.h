/*
 * Draw.h
 *
 *  Created on: 2018-3-22
 *      Author: Administrator
 */

#ifndef DRAW_H_
#define DRAW_H_

#define X_Start			1
#define X_End			240
#define Y_Start			1
#define Y_End			160

#define Row_X			236
#define Row_Y_Start		20
#define Row_Y_End		160
#define Row_High		139

#define PutSMLAsc(x,y,pData,wLen)  Put0816Asc(x,y,pData,wLen,FALSE)

int wLenOfStr(char *pData);
void InverArea(int x1,int y1,int x2,int y2);
void Line(int x1,int y1,int x2,int y2);
void PutPixel(int x,int y);
void PutRoll(int Postive,int ItemNum);
void YLine(int x,int y1,int y2);
void XLine(int x1,int x2,int y);
void RectangleFill(int x1,int y1,int x2,int y2);
void Rectangle(int x1,int y1,int x2,int y2);
void PutPic(int xStart,int yStart,int wide,int high,const char *pData,bool wInv);
void Put0612Asc(int x,int y,char *pData,int wLen,int Space);
void Put0816Asc(int x,int y,char *pData,int wLen,bool wInv);
void Put2232Asc(int x,int y,char *pData,int wLen,bool wInv);
void PutHz(int x,int y ,char *pData,int wLen,bool wInv);

void ClearArea(int x1,int y1,int x2,int y2);
void CloudDisp(int x,int y);
void RainDisp(int x,int y);
void SunDisp(int x,int y);
s32 wPow(s32 data,u16 p);
#endif /* DRAW_H_ */
