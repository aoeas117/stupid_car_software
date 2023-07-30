#ifndef _yao_gan_h
#define _yao_gan_h

#include "headfile.h"


#define SERVO_MID 750 //中值pwm
#define SERVO_STEP -250 //舵机打角幅度   MIN:480 MAX:960

#define angle_limit 5//角度限幅 角度
#define speed_limit 120//速度限幅 占空比

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
u8 ps2_angle(u16 x);//返回0-180角度
u16 ps2_speed(u16 pointer,u16 speed_scale);
void ps2_limit(void);
void key_scan(void);


#endif