#ifndef _touch_h
#define _touch_h
#include "common.h"


#define TOUCH_CS B13
#define TOUCH_CLK B12	
#define TOUCH_BUSY C6
#define TOUCH_PEN C7
#define TOUCH_MI B15
#define TOUCH_MO B14
//CMDָ��
#define CMD_RDX 0XD0//��x����
#define CMD_RDY 0X90//��y����
/*****��λ*****/
#define TOUCH_CLK_SET gpio_set(TOUCH_CLK,1)
#define TOUCH_MO_SET gpio_set(TOUCH_MO,1)
#define TOUCH_CS_SET gpio_set(TOUCH_CS,1)
/*****��λ*****/
#define TOUCH_CLK_CLE gpio_set(TOUCH_CLK,0)
#define TOUCH_MO_CLE gpio_set(TOUCH_MO,0)
#define TOUCH_CS_CLE gpio_set(TOUCH_CS,0)
/*****������*****/
#define TOUCH_MI_GET gpio_get(TOUCH_MI)
#define TOUCH_PEN_GET gpio_get(TOUCH_PEN)
//У׼
#define TP_PRES_DOWN 0x80  		//����������	  
#define TP_CATH_PRES 0x40  		//�а���������

#define abs(x) x>0?x:-x



typedef struct
{
	u16 x_current;
	u16 y_current;
	u16 x_last;
	u16 y_last;
}TP_POS;
//״̬��
typedef struct
{
	u8 tp_press_turn : 1; //��ʾ���°�������  1��δ����   0��������
	u8 tp_release_turn : 1;//��ʾ�ɿ��������� 1��δ����   0��������
	u8 tp_sta_current : 1;//��ʾ����״̬      1������     0���ɿ�
}TP_STA;



extern TP_STA tp_sta;
extern TP_POS tp_pos;



void touch_pin_init();//���������ų�ʼ��
void TP_Write_Byte(uint8 num);//������spiд�ֽ�
uint16 TP_Read_AD(uint8 CMD);//
uint16 TP_Read_XOY(uint8 xy);//��ȡx��y���������겢�˲�
uint8 TP_Read_XY(uint16 *x,uint16 *y);//��ȡx��y����
uint8 TP_Read_XY2(uint16 *x,uint16 *y);//������ȡ����xy���겢�ж��Ƿ�ϸ�

void TP_Init(void);
/*****************/
void tp_scan(void);








#endif