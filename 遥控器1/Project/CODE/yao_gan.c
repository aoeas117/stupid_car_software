#include "common.h"
#include "yao_gan.h"
#include "rov_page.h"
#include "mw1208.h"


u16 adc_buff[3]; //adc采样值




void tim_init(u8 frequent)
{
	tim_interrupt_init(TIM_3,frequent,0);
}


void tim_switch(u8 mode)
{
	if(mode)
		tim_interrupt_enable(TIM_3);
	else
		tim_interrupt_disable(TIM_3);


}

u16 mid_angle_x=0,mid_angle_y=0;//电位器中间采样值
u16 mid_speed=0;//电位器采样中值



void ps2_init(void)
{
  adc_init(ADC_1, ADC1_CH05_A05, ADC_12BIT);										// 初始化ADC1通道x引脚精度12bit
  adc_init(ADC_1, ADC1_CH06_A06, ADC_12BIT);
  adc_init(ADC_1, ADC1_CH08_B00, ADC_12BIT);
  adc_init(ADC_1, ADC1_CH09_B01, ADC_12BIT);
	
	//gpio_init(C4,GPI,1,GPI_PULL_UP);
	//gpio_init(C5,GPI,1,GPI_PULL_UP);
	
	tim_init(8);
	tim_switch(0);
	mid_angle_x=adc_mean_filter(ADC_1,ADC2_CH05_A05,5);
	mid_angle_y=adc_mean_filter(ADC_1,ADC2_CH06_A06,5);
	mid_speed  =adc_mean_filter(ADC_1,ADC2_CH09_B01,5);

}


void key_scan(void)
{
	static u8 key_up=1;//按键按松开标志		  
	if(key_up&&(gpio_get(C4)==0 || gpio_get(C5)==0))
	{
		systick_delay_ms(10);//去抖动 
		key_up=0;
		if(gpio_get(C4)==0)
		{
				u8 brake=0;
				encode_lora_cmd(motor_brake,&brake,unsigned_char_type);
		}
		else if(gpio_get(C5)==0)
		{
				u8 brake=1;
				encode_lora_cmd(motor_brake,&brake,unsigned_char_type);
		}
					
		else return;
		
	}
	else if(gpio_get(C4)==1&& gpio_get(C5)==1)key_up=1; 	     
	
}
















/****角度****/




u8 angle[2]={90,90};//保存上次与当前角度，便于限幅滤波



u8 ps2_angle(u16 x)//返回0-180角度
{

	float x0=mid_angle_x-x;
	x0=x0/mid_angle_x;
	if(x0>0.3 || x0<-0.3)
	return -x0*90+90;
	return 90;
}
/**********速度************/


u16 speed[2]={0,0};//保存上次与当前速度，便于限幅滤波




u16 ps2_speed(u16 pointer,u16 speed_scale)//返回速度
{

	float p0=mid_speed-pointer;
	p0=p0/mid_speed;
	
	if(p0>0.2)
		return (p0+0.5)*speed_scale;
	return 0;
}








void ps2_limit(void)
{
	if(angle[0]>150)
		angle[0]=150;
	else if(angle[0]<30)
		angle[0]=30;
	
	if(speed[0]>speed_gear)
		speed[0]=speed_gear;
	
	int delta=angle[0]-angle[1];
	if(delta<0)
		delta=-delta;
	if(angle[0]!=90)
	{
		if(delta>angle_limit)//限幅,提高响应，抑制突变
		{
			if(angle[0]>angle[1])
				angle[0]=angle[1]+angle_limit;
			else 
				angle[0]=angle[1]-angle_limit;
		}
	}
	angle[1]=angle[0];
	
	delta=speed[0]-speed[1];
	if(delta<0)
		delta=-delta;
	if(delta>speed_limit)
	{
		if(speed[0]>speed[1])
			speed[0]=speed[1]+speed_limit;
		else 
			speed[0]=speed[1]-speed_limit;
		speed[1]=speed[0];
	}
	
	

}




u16 send_servo_pwm=SERVO_MID;
u16 send_motor_pwm=0;
u16 speed_gear=600;
C_rov c_rov={.speed_scan=0,.angle_scan=0,.angle_indicator=0,.speed_indicator=0};




void TIM3_IRQHandler (void)
{
	uint32 state = TIM3->SR;														// 读取中断状态
	TIM3->SR &= ~state;																// 清空中断状态
//	
	adc_buff[0]=adc_mean_filter(ADC_1,ADC2_CH05_A05,5);
		adc_buff[1]=adc_mean_filter(ADC_1,ADC2_CH06_A06,5);
	adc_buff[2]=adc_mean_filter(ADC_1,ADC1_CH09_B01,5);
	
	angle[0]=ps2_angle(adc_buff[0]);
	speed[0]=ps2_speed(adc_buff[2],speed_gear);
	
	
	
	
	
	ps2_limit();//限幅
	
	if(c_rov.angle_indicator)
		lcd_lmeter_set_angle(Rov_page.lmeter1,angle[0]);
	if(c_rov.speed_indicator)
		lcd_bar_set_progress(Rov_page.bar1,(float)speed[0]/(float)speed_gear);
	
	send_servo_pwm=((float)(angle[0]-90)/60)*SERVO_STEP+SERVO_MID;
	send_motor_pwm=speed[0];
	
	
	
	/*****发送更新指令*******/
	
	if(c_rov.angle_scan)
		encode_lora_cmd(setangle,&send_servo_pwm,unsigned_short_type);
	if(c_rov.speed_scan)
		encode_lora_cmd(setspeed,&send_motor_pwm,short_type);
	
	
	
	
	
	/*************/
	
	
	
}