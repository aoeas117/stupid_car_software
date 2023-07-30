/*
 * gtn_pwm.h
 *
 *  Created on: 2023年3月3日
 *      Author: 乘衣归
 */

#ifndef CODE_GTM_PWM_H_
#define CODE_GTM_PWM_H_

#include "zf_common_headfile.h"



/******************define****************************/

#define CMU_CLK_ALL_DISABLE     (0x55555555)    // 时钟全部失能
#define CMU_CLK_ENABLE          (2)             // 时钟使能



#define MOTOR_SCOPE 100





#define COMMUTATION_TIMEOUT 5000
#define FCY                     ((uint32)100000000)                 // 模块时钟
#define FPWM                    ((uint16)20000)                     // PWM频率
#define PWM_PRIOD_LOAD          (uint16)(5000)                      // PWM周期装载值
#define POLEPAIRS               (1)                                 // 电机极对数



#define VALID_IN_PATTERN        (1u)
#define INVALID_IN_PATTERN      (0u)


#define HALL_HARDWARE_ORDER_0   (3u)
#define HALL_HARDWARE_ORDER_1   (1u)
#define HALL_HARDWARE_ORDER_2   (5u)
#define HALL_HARDWARE_ORDER_3   (4u)
#define HALL_HARDWARE_ORDER_4   (6u)
#define HALL_HARDWARE_ORDER_5   (2u)



// 电机正转
#define MOTOR_FOREWARD_0        (0xEA8A)    // A上桥C下桥开启，其他关闭
#define MOTOR_FOREWARD_1        (0xE8AA)    // B上桥C下桥开启，其他关闭
#define MOTOR_FOREWARD_2        (0xA8EA)    // B上桥A下桥开启，其他关闭
#define MOTOR_FOREWARD_3        (0x8AEA)    // C上桥A下桥开启，其他关闭
#define MOTOR_FOREWARD_4        (0x8EAA)    // C上桥B下桥开启，其他关闭
#define MOTOR_FOREWARD_5        (0xAE8A)    // A上桥B下桥开启，其他关闭
// 电机反转
#define MOTOR_REVERSAL_0        (0xA8EA)    // B上桥A下桥开启，其他关闭
#define MOTOR_REVERSAL_1        (0x8AEA)    // C上桥A下桥开启，其他关闭
#define MOTOR_REVERSAL_2        (0x8EAA)    // C上桥B下桥开启，其他关闭
#define MOTOR_REVERSAL_3        (0xAE8A)    // A上桥B下桥开启，其他关闭
#define MOTOR_REVERSAL_4        (0xEA8A)    // A上桥C下桥开启，其他关闭
#define MOTOR_REVERSAL_5        (0xE8AA)    // B上桥C下桥开启，其他关闭

#define MOTOR_BRAKE             (0xEEEA)    // 刹车



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


// TOM输出信号模式选择
#define PAT_PTR_0               (0u)
#define PAT_PTR_1               (1u)
#define PAT_PTR_2               (2u)
#define PAT_PTR_3               (3u)
#define PAT_PTR_4               (4u)
#define PAT_PTR_5               (5u)



#define TOM_CH_CMU_FXCLK0       (0)


#define FORCE_UPDATE_EN         (2)         //  输出控制寄存器强制更新
#define FORCE_UPDATE_DIS        (1)         //  忽略更新


#define TOM_OUTPUT_ENABLE       (2)         //  输出通道使能
#define TOM_OUTPUT_DISABLE      (1)         //  输出通道失能


#define ENDIS_EN                (2)         //  更新使能
#define ENDIS_DIS               (1)         //  更新失能


#define SRC_SPE_INT_PRIO        (90)
#define SRC_PULSE_NOTIFY        (2U)
#define SRC_ENABLE_ALL          (0x1F)
#define SRC_CPU0                (0)
#define SRC_CPU1                (1)
#define SRC_ENABLE              (1)


#define REV_CNT_VAL             (10u)       // 换相固定次数触发中断
#define FILTER_CON_NUM          (10u)       // 霍尔信号的滤波时长，主要作用是去除毛刺





// 以下引脚为实际输入输出引脚
#define HALL_A                  (IfxGtm_TIM0_1_TIN35_P33_13_IN )    // 霍尔信号A相输入
#define HALL_B                  (IfxGtm_TIM0_0_TIN34_P33_12_IN )    // 霍尔信号B相输入
#define HALL_C                  (IfxGtm_TIM0_2_TIN33_P33_11_IN )    // 霍尔信号C相输入
#define FSOI_PIN                (IfxGtm_TIM1_6_TIN66_P20_10_IN )    // 刹车信号输入（快速关闭）
#define A_PHASE_PIN_H           (IfxGtm_TOM0_2_TOUT28_P33_6_OUT)    // 电机A相上桥控制引脚
#define A_PHASE_PIN_L           (IfxGtm_TOM0_3_TOUT29_P33_7_OUT)    // 电机A相下桥控制引脚
#define B_PHASE_PIN_H           (IfxGtm_TOM0_4_TOUT30_P33_8_OUT)    // 电机B相上桥控制引脚
#define B_PHASE_PIN_L           (IfxGtm_TOM0_5_TOUT40_P32_4_OUT)    // 电机B相下桥控制引脚
#define C_PHASE_PIN_H           (IfxGtm_TOM0_6_TOUT42_P23_1_OUT)    // 电机B相上桥控制引脚
#define C_PHASE_PIN_L           (IfxGtm_TOM0_7_TOUT64_P20_8_OUT)    // 电机B相下桥控制引脚






#define MOTOR_MAX_DUTY 800
#define MOTOR_MIN_DUTY 0
#define I_OUTPUT_MAX   1000
#define I_OUTPUT_MIN   -1000




/****************typedef****************************/



typedef struct
{
        struct
        {
               int16 target_speed;//目标速度:rpm (正为正转，负为反转)
               int16 current_speed;//当前速度:rpm
               int16 last_speed;   //上次速度:rpm
               /********当前电机工作状态*******/
               uint8 motor_dir : 1;//转子方向（1：正转 0：反转）
               uint8 motor_brake :1;//1
               int32 motor_duty_output;//电机输出占空比
               int32 last_motor_duty_output;//上次电机输出占空比
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

       uint8 control_mode :1;//1：开环 0：闭环


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
