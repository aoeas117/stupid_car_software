/*
 * gtn_pwm.h
 *
 *  Created on: 2023��3��3��
 *      Author: ���¹�
 */

#ifndef CODE_GTM_PWM_H_
#define CODE_GTM_PWM_H_

#include "zf_common_headfile.h"



/******************define****************************/

#define CMU_CLK_ALL_DISABLE     (0x55555555)    // ʱ��ȫ��ʧ��
#define CMU_CLK_ENABLE          (2)             // ʱ��ʹ��



#define MOTOR_SCOPE 100





#define COMMUTATION_TIMEOUT 5000
#define FCY                     ((uint32)100000000)                 // ģ��ʱ��
#define FPWM                    ((uint16)20000)                     // PWMƵ��
#define PWM_PRIOD_LOAD          (uint16)(5000)                      // PWM����װ��ֵ
#define POLEPAIRS               (1)                                 // ���������



#define VALID_IN_PATTERN        (1u)
#define INVALID_IN_PATTERN      (0u)


#define HALL_HARDWARE_ORDER_0   (3u)
#define HALL_HARDWARE_ORDER_1   (1u)
#define HALL_HARDWARE_ORDER_2   (5u)
#define HALL_HARDWARE_ORDER_3   (4u)
#define HALL_HARDWARE_ORDER_4   (6u)
#define HALL_HARDWARE_ORDER_5   (2u)



// �����ת
#define MOTOR_FOREWARD_0        (0xEA8A)    // A����C���ſ����������ر�
#define MOTOR_FOREWARD_1        (0xE8AA)    // B����C���ſ����������ر�
#define MOTOR_FOREWARD_2        (0xA8EA)    // B����A���ſ����������ر�
#define MOTOR_FOREWARD_3        (0x8AEA)    // C����A���ſ����������ر�
#define MOTOR_FOREWARD_4        (0x8EAA)    // C����B���ſ����������ر�
#define MOTOR_FOREWARD_5        (0xAE8A)    // A����B���ſ����������ر�
// �����ת
#define MOTOR_REVERSAL_0        (0xA8EA)    // B����A���ſ����������ر�
#define MOTOR_REVERSAL_1        (0x8AEA)    // C����A���ſ����������ر�
#define MOTOR_REVERSAL_2        (0x8EAA)    // C����B���ſ����������ر�
#define MOTOR_REVERSAL_3        (0xAE8A)    // A����B���ſ����������ر�
#define MOTOR_REVERSAL_4        (0xEA8A)    // A����C���ſ����������ر�
#define MOTOR_REVERSAL_5        (0xE8AA)    // B����C���ſ����������ر�

#define MOTOR_BRAKE             (0xEEEA)    // ɲ��



#define TOM_CH_SL_LOW           (0)
#define TOM_CH_SL_HIGH          (1)
#define OSM_EN                  (1)
#define OSM_DIS                 (0)
#define SPEM_EN                 (1)
#define SPEM_DIS                (0)
#define CM0_MATCHING            (0)
#define TRIG_0_MATCHING         (1)
#define TRIG_0_EXT_TRIG         (0)

#define TRIG_CCU0               (1)
#define TRIG_BEFORE             (0)


// TOM����ź�ģʽѡ��
#define PAT_PTR_0               (0u)
#define PAT_PTR_1               (1u)
#define PAT_PTR_2               (2u)
#define PAT_PTR_3               (3u)
#define PAT_PTR_4               (4u)
#define PAT_PTR_5               (5u)



#define TOM_CH_CMU_FXCLK0       (0)


#define FORCE_UPDATE_EN         (2)         //  ������ƼĴ���ǿ�Ƹ���
#define FORCE_UPDATE_DIS        (1)         //  ���Ը���


#define TOM_OUTPUT_ENABLE       (2)         //  ���ͨ��ʹ��
#define TOM_OUTPUT_DISABLE      (1)         //  ���ͨ��ʧ��


#define ENDIS_EN                (2)         //  ����ʹ��
#define ENDIS_DIS               (1)         //  ����ʧ��


#define SRC_SPE_INT_PRIO        (90)
#define SRC_PULSE_NOTIFY        (2U)
#define SRC_ENABLE_ALL          (0x1F)
#define SRC_CPU0                (0)
#define SRC_CPU1                (1)
#define SRC_ENABLE              (1)


#define REV_CNT_VAL             (10u)       // ����̶����������ж�
#define FILTER_CON_NUM          (10u)       // �����źŵ��˲�ʱ������Ҫ������ȥ��ë��





// ��������Ϊʵ�������������
#define HALL_A                  (IfxGtm_TIM0_1_TIN35_P33_13_IN )    // �����ź�A������
#define HALL_B                  (IfxGtm_TIM0_0_TIN34_P33_12_IN )    // �����ź�B������
#define HALL_C                  (IfxGtm_TIM0_2_TIN33_P33_11_IN )    // �����ź�C������
#define FSOI_PIN                (IfxGtm_TIM1_6_TIN66_P20_10_IN )    // ɲ���ź����루���ٹرգ�
#define A_PHASE_PIN_H           (IfxGtm_TOM0_2_TOUT28_P33_6_OUT)    // ���A�����ſ�������
#define A_PHASE_PIN_L           (IfxGtm_TOM0_3_TOUT29_P33_7_OUT)    // ���A�����ſ�������
#define B_PHASE_PIN_H           (IfxGtm_TOM0_4_TOUT30_P33_8_OUT)    // ���B�����ſ�������
#define B_PHASE_PIN_L           (IfxGtm_TOM0_5_TOUT40_P32_4_OUT)    // ���B�����ſ�������
#define C_PHASE_PIN_H           (IfxGtm_TOM0_6_TOUT42_P23_1_OUT)    // ���B�����ſ�������
#define C_PHASE_PIN_L           (IfxGtm_TOM0_7_TOUT64_P20_8_OUT)    // ���B�����ſ�������






#define MOTOR_MAX_DUTY 800
#define MOTOR_MIN_DUTY 0
#define I_OUTPUT_MAX   1000
#define I_OUTPUT_MIN   -1000




/****************typedef****************************/



typedef struct
{
        struct
        {
               int16 target_speed;//Ŀ���ٶ�:rpm (��Ϊ��ת����Ϊ��ת)
               int16 current_speed;//��ǰ�ٶ�:rpm
               int16 last_speed;   //�ϴ��ٶ�:rpm
               /********��ǰ�������״̬*******/
               uint8 motor_dir : 1;//ת�ӷ���1����ת 0����ת��
               uint8 motor_brake :1;//1
               int32 motor_duty_output;//������ռ�ձ�
               int32 last_motor_duty_output;//�ϴε�����ռ�ձ�
               uint32 motor_period_output;
               /***************************/
               uint16 max_duty;

        }motor_speed;

       struct
       {
               float p;
               float i;
               float d;
               int16 current_error;
               int16 last_error;
               int16 output;
               float i_output;
       }pid;
       struct
       {
               uint32 time_save[6];
               uint8 phase;
               uint32 time_sum;

       }get_speed;

       uint8 control_mode :1;//1������ 0���ջ�


}MOTOR_Administer;


extern MOTOR_Administer*motor;








static void clock_cfg               (void);
static void ccm_cfg                 (void);
static void hall_tim_input_cfg      (void);
static void spe0_irq_init           (void);
static void spe0_cfg                (void);
static void spe0_pattern_init_cfg   (void);
       void spe0_pattern_cfg        (void);
static void spe0_tom_tgc_cfg        (void);
static void spe0_pwm_tom_cfg        (void);
static void spe0_pwm_mux_outport_cfg(void);
       void gtm_bldc_init           (void);
/********************************************/

       void motor_administer_init       (void);
       void motor_set_pwm               (MOTOR_Administer *MA);
       void motor_set_dir               (MOTOR_Administer *MA);
       void motor_get_speed             (MOTOR_Administer *MA);
       void motor_control_speed_output  (MOTOR_Administer *MA);
       void motor_set_target_speed      (MOTOR_Administer *MA,int16 target_speed);
       void motor_set_pwm_from_out      (MOTOR_Administer *MA,uint16 target_pwm);
       void motor_set_stop              (MOTOR_Administer *MA,uint8 brake);
       void phase_change_time_check     (MOTOR_Administer *MA);
       void motor_set_pwm_ud            (MOTOR_Administer *MA,int16 target_pwm);
       void motor_open_loop_switch_dir  (MOTOR_Administer *MA);
       void motor_switch_control_mode   (MOTOR_Administer *MA);



















#endif /* CODE_GTM_PWM_H_ */
