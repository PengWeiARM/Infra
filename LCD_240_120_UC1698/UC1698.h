/*
 * UC1698.h
 *
 *  Created on: 2018-2-2
 *      Author: Administrator
 */

#ifndef UC1698_H_
#define UC1698_H_

#include "typedef.h"
#include "macro.h"


#define LcdWide 240
#define LcdHigh	160

/*
	CS---PF12
	A0---PF11

	WR---PF10
	RD---PF9

	DB0---PF7
	DB1---PF6
	DB2---PF5
	DB3---PF4
	DB4---PF3
	DB5---PF2
	DB6---PF1
	DB7---PF0

	BLA---PA15
*/

#define lcdLed_On()	  GPIO_Back_Led_On()
#define lcdLed_Off()  GPIO_Back_Led_Off()

#define CS_H()      GPIO_CS_HIGH()
#define CS_L()      GPIO_CS_LOW()
#define RS_H()      GPIO_A0_HIGH()
#define RS_L()      GPIO_A0_LOW()
#define WR_H()      GPIO_WR_HIGH()
#define WR_L()      GPIO_WR_LOW()
#define RD_H()      GPIO_RD_HIGH()
#define RD_L()      GPIO_RD_LOW()
#define RST_H()      GPIO_RST_HIGH()
#define RST_L()      GPIO_RST_LOW()

#define WR_Data(x)  GPIO_WR_DATA(x)



#define 	BufLen			2400


#define Start_Seg		0//0 //0x25	//SEG0  		x:SEG0~SEG240
#define End_Seg			0xf0//0xf0 //0x5a	//SEG240
#define Start_Com		120//120		//					y:COM120~COM1



#define CMD		0
#define DATA	1


extern u16 DispBuf[160][15];
void InitLCDM(void);
void ClearBuf(void);
void UpdateDispBuff(void);

#endif /* UC1698_H_ */
