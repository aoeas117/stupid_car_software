/*
 * adc.h
 *
 *  Created on: 2023年3月4日
 *      Author: 乘衣归
 */

#ifndef CODE_ADC_H_
#define CODE_ADC_H_

#include "zf_common_headfile.h"




#define CU_A        ADC2_CH3_A35
#define CU_B        ADC2_CH4_A36
#define CU_SUM      ADC2_CH6_A38
#define VO_POWER    ADC2_CH12_A46
#define CURRENT_TRANSITION_VALUE 0.00537

#define MAX_CURRENT 30//最大电流
#define MIN_VOLT    14.9//最小电压

#define JUDGE_TIME 100// 判断时间





typedef struct
{
        float current_a;
        float current_b;
        float current_sum;
        float volt_power;

        uint8 high_current :1;//过流标志  1:过流
        uint16 current_count;//过流时间

        uint8 volt_sta :2;//电压状态 0：普通电压 1：低电压 2：刹车电压
        uint16 volt_count ;//电压时间


}ADC_INFORMATION;

extern  ADC_INFORMATION adc_information;



void adc_information_init(void);
void adc_information_read(void);
void adc_information_check(void);















#endif /* CODE_ADC_H_ */
