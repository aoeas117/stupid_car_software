#ifndef _touch_h
#define _touch_h
#include "common.h"


#define TOUCH_CS B13
#define TOUCH_CLK B12	
#define TOUCH_BUSY C6
#define TOUCH_PEN C7
#define TOUCH_MI B15
#define TOUCH_MO B14
//CMD指令
#define CMD_RDX 0XD0//读x坐标
#define CMD_RDY 0X90//读y坐标
/*****置位*****/
#define TOUCH_CLK_SET gpio_set(TOUCH_CLK,1)
#define TOUCH_MO_SET gpio_set(TOUCH_MO,1)
#define TOUCH_CS_SET gpio_set(TOUCH_CS,1)
/*****复位*****/
#define TOUCH_CLK_CLE gpio_set(TOUCH_CLK,0)
#define TOUCH_MO_CLE gpio_set(TOUCH_MO,0)
#define TOUCH_CS_CLE gpio_set(TOUCH_CS,0)
/*****读引脚*****/
#define TOUCH_MI_GET gpio_get(TOUCH_MI)
#define TOUCH_PEN_GET gpio_get(TOUCH_PEN)
//校准
#define TP_PRES_DOWN 0x80  		//触屏被按下	  
#define TP_CATH_PRES 0x40  		//有按键按下了

#define abs(x) x>0?x:-x



typedef struct
{
	u16 x_current;
	u16 y_current;
	u16 x_last;
	u16 y_last;
}TP_POS;
//状态量
typedef struct
{
	u8 tp_press_turn : 1; //表示按下按键处理  1；未处理   0：处理了
	u8 tp_release_turn : 1;//表示松开按键处理 1：未处理   0：处理了
	u8 tp_sta_current : 1;//表示触屏状态      1：按下     0：松开
}TP_STA;



extern TP_STA tp_sta;
extern TP_POS tp_pos;



void touch_pin_init();//触摸屏引脚初始化
void TP_Write_Byte(uint8 num);//触摸屏spi写字节
uint16 TP_Read_AD(uint8 CMD);//
uint16 TP_Read_XOY(uint8 xy);//读取x或y的物理坐标并滤波
uint8 TP_Read_XY(uint16 *x,uint16 *y);//读取x和y坐标
uint8 TP_Read_XY2(uint16 *x,uint16 *y);//连续读取两次xy坐标并判断是否合格

void TP_Init(void);
/*****************/
void tp_scan(void);








#endif