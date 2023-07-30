#ifndef _yao_gan_h
#define _yao_gan_h

#include "headfile.h"


#define SERVO_MID 750 //��ֵpwm
#define SERVO_STEP -250 //�����Ƿ���   MIN:480 MAX:960

#define angle_limit 5//�Ƕ��޷� �Ƕ�
#define speed_limit 120//�ٶ��޷� ռ�ձ�

extern u16 speed_gear;


typedef struct
{
	u8 speed_scan :1;
	u8 angle_scan :1;
	u8 speed_indicator :1;
	u8 angle_indicator :1;
}C_rov;


extern C_rov c_rov;




void ps2_init(void);
void tim_init(u8 frequent);
void tim_switch(u8 mode);
u8 ps2_angle(u16 x);//����0-180�Ƕ�
u16 ps2_speed(u16 pointer,u16 speed_scale);
void ps2_limit(void);
void key_scan(void);


#endif