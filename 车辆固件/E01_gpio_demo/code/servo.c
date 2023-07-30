/*
 * servo.c
 *
 *  Created on: 2023年2月27日
 *      Author: 乘衣归
 */

#include "servo.h"
#include "gps.h"

#define KEY P00_12

SERVO_Administrators servo;


void servo_init(uint16 fred)
{
    pwm_init(SERVO_PWM_CH,fred,SERVO_MID);
    servo.pid.p=9.0;
    //servo.pid.i=0.0;
    servo.pid.d=1.0;
    //servo.pid.error_sum=0;
    servo.pid.target_yaw=180;
    servo.pid.current_error=0.0;
    servo.pid.output_pwm=0;

    gpio_init(KEY,GPI,1,GPI_PULL_UP);



}

void servo_set_pwm(uint16 pwm)
{
    if(pwm>SERVO_MID+SERVO_STEP)
        pwm=SERVO_MID+SERVO_STEP;
    if(pwm<SERVO_MID-SERVO_STEP)
        pwm=SERVO_MID-SERVO_STEP;
    pwm_set_duty(SERVO_PWM_CH,pwm);
}




void servo_get_target_yaw(SERVO_Administrators *SA,GPS_Administrator *GA,uint8 target_p)
{
    SA->pid.target_yaw=get_two_points_azimuth(SA->latitude,SA->longitude,GA->latitude_point[target_p],GA->longitude_point[target_p]);
}


void servo_control_angle_output(SERVO_Administrators *SA)
{
    SA->pid.current_yaw=att_data.yaw;//(0~360)
    SA->pid.current_error=SA->pid.target_yaw-SA->pid.current_yaw;
    //SA->pid.error_sum+=SA->pid.current_error;
    if(SA->pid.current_error>180) SA->pid.current_error-=360;
    if(SA->pid.current_error<-180) SA->pid.current_error+=360;

    SA->pid.output_pwm=SERVO_MID+SA->pid.current_error*SA->pid.p-(SA->pid.current_error-SA->pid.last_error)*SA->pid.d;

    SA->pid.last_error=SA->pid.current_error;

    servo_set_pwm(SA->pid.output_pwm);
}




void key_scan(void)
{
    static uint8 key_up=1;
    if(key_up && gpio_get_level(KEY)==0)
    {
        system_delay_ms(10);
        key_up=0;
        if(gpio_get_level(KEY)==0)
        {
            system_delay_ms(3000);//延时

            //pwm_set_duty(SERVO_PWM_CH,SERVO_MID);

            motor->control_mode=0;//电机闭环
            control.imu_solution_flag=1;//舵机闭环

            control.search_point_flag=1;
            control.target_point=0;
            control.whole_test=1;
            gps_asistant_flag=0;

            //解除刹车
            motor_set_stop(motor,0);

        }else return;
    }else if(gpio_get_level(KEY)) key_up=1;

}









