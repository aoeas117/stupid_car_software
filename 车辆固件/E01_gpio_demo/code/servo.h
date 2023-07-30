/*
 * servo.h
 *
 *  Created on: 2023Äê2ÔÂ27ÈÕ
 *      Author: ³ËÒÂ¹é
 */

#ifndef CODE_SERVO_H_
#define CODE_SERVO_H_

#include "zf_common_headfile.h"


#define SERVO_PIN P10_3
#define SERVO_PWM_CH ATOM1_CH3_P10_3
#define SERVO_MID 750

#define SERVO_STEP 250



typedef struct
{
        struct
        {
                float p;
                //float i;
                float d;
                //float error_sum;
                float current_yaw;
                float target_yaw;
                float current_error;
                float last_error;
                uint16 output_pwm;
        }pid;

        float latitude;
        float longitude;


}SERVO_Administrators;



void servo_init(uint16 fred);
void servo_set_pwm(uint16 pwm);
void servo_control_angle_output(SERVO_Administrators *SA);

void key_scan(void);









extern SERVO_Administrators servo;







#endif /* CODE_SERVO_H_ */
