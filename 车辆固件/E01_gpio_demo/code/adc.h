/*
 * adc.h
 *
 *  Created on: 2023��3��4��
 *      Author: ���¹�
 */

#ifndef CODE_ADC_H_
#define CODE_ADC_H_

#include "zf_common_headfile.h"




#define CU_A        ADC2_CH3_A35
#define CU_B        ADC2_CH4_A36
#define CU_SUM      ADC2_CH6_A38
#define VO_POWER    ADC2_CH12_A46
#define CURRENT_TRANSITION_VALUE 0.00537

#define MAX_CURRENT 30//������
#define MIN_VOLT    14.9//��С��ѹ

#define JUDGE_TIME 100// �ж�ʱ��





typedef struct
{
        float current_a;
        float current_b;
        float current_sum;
        float volt_power;

        uint8 high_current :1;//������־  1:����
        uint16 current_count;//����ʱ��

        uint8 volt_sta :2;//��ѹ״̬ 0����ͨ��ѹ 1���͵�ѹ 2��ɲ����ѹ
        uint16 volt_count ;//��ѹʱ��


}ADC_INFORMATION;

extern  ADC_INFORMATION adc_information;



void adc_information_init(void);
void adc_information_read(void);
void adc_information_check(void);















#endif /* CODE_ADC_H_ */
