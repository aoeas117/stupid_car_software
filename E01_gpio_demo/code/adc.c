/*
 * adc.c
 *
 *  Created on: 2023��3��4��
 *      Author: ���¹�
 */


#include "adc.h"


ADC_INFORMATION adc_information;

void adc_information_init(void)
{
    adc_init(CU_A,ADC_12BIT);
    adc_init(CU_B,ADC_12BIT);
    adc_init(CU_SUM,ADC_12BIT);
    adc_init(VO_POWER,ADC_12BIT);

    adc_information.high_current=0;//δ����
    adc_information.current_count=0;
    adc_information.volt_sta=0;//��ͨ��ѹ
    adc_information.volt_count=0;
}




void adc_information_read(void)
{
        uint16 adc_value_a_phase        = 0;
        uint16 adc_value_b_phase        = 0;
        uint16 adc_value_bus_phase      = 0;
        uint16 adc_value_power_phase    = 0;



        adc_value_a_phase               = adc_convert(CU_A          );                                  // ��ȡA������ɼ��˿ڵ�ֵ
        adc_value_b_phase               = adc_convert(CU_B          );                                  // ��ȡA������ɼ��˿ڵ�ֵ
        adc_value_bus_phase             = adc_convert(CU_SUM        );                                  // ��ȡĸ�ߵ����ɼ��˿ڵ�ֵ
        adc_value_power_phase           = adc_convert(VO_POWER      );

        adc_information.current_a       = (float)(adc_value_a_phase   - 2048)   * CURRENT_TRANSITION_VALUE;     // ����A�����
        adc_information.current_b       = (float)(adc_value_b_phase   - 2048)   * CURRENT_TRANSITION_VALUE;     // ����B�����
        adc_information.current_sum     = (float)((adc_value_bus_phase - 2048)   * CURRENT_TRANSITION_VALUE)*48.0/75.0;     // ����ĸ�ߵ���
        adc_information.volt_power      = 3.3*6.15*(float)adc_value_power_phase/4096.0;

        //char string[10];
        //sprintf(string,"%f", adc_information.volt_power );
       //lora_send_str(&lora,string);



}



void adc_information_check(void)
{

    if(adc_information.current_sum<0)
        adc_information.current_sum=-adc_information.current_sum;

    if(adc_information.current_sum> MAX_CURRENT)
    {
        adc_information.current_count++;
    }else
    {
        adc_information.current_count=(adc_information.current_count > 100 && adc_information.current_count < 300)? (adc_information.current_count + 1) : 0;

    }

    if(adc_information.volt_power<MIN_VOLT)
    {
           adc_information.volt_count++;
    }else
    {
        adc_information.volt_count=(adc_information.volt_count > 100 && adc_information.volt_count < 300)? (adc_information.volt_count + 1) : 0;
    }

    if(motor->motor_speed.motor_brake==0)//δɲ��
    {

        if( adc_information.current_count>JUDGE_TIME)
        {
            motor_set_stop(motor,1);
            lora_send_str(&lora,"brake(current)");

        }

        if(adc_information.volt_count>JUDGE_TIME)
        {
            motor_set_stop(motor,1);
            lora_send_str(&lora,"brake(volt)");
        }
    }




}






