/*
 * lora.h
 *
 *  Created on: 2023��1��13��
 *      Author: ���¹�
 */

#ifndef CODE_LORA_H_
#define CODE_LORA_H_
/*********include*************/

#include "zf_common_headfile.h"

/**********define************/

#define PIN_LORA_AUX                  P02_4
#define PIN_LORA_MD0                  P02_5
#define PIN_LORA_TX                   UART2_TX_P02_0
#define PIN_LORA_RX                   UART2_RX_P02_1
#define LORA_UART                     UART_2
#define LORA_BAUD                     115200
#define LORA_SEND_BUFFER_SIZE         32
#define LORA_RECEIVE_BUFFER_SIZE      32
#define TOTAL_INTERNAL_DATA_SIZE      32


/***********typedef************/


extern int16 motor_pwm_up;


typedef enum
{
    data_receiving=0,
    data_recevie_unuse,
    data_receive_used,
}LORA_RECEIVE_STA;


typedef enum
{
    void_type=0,
    float_type,
    string_type,
    double_type,
    unsigned_char_type,//u8
    unsigned_short_type,//u16
    unsigned_long_type,//u32
    unsigned_int_type,
    char_type,
    long_type,
    short_type,
    int_type,
    long_long_type,
}DATA_TYPE;//��������ö��


typedef enum
{

    void_data=32,
    motor_pwm,
    motor_p,
    motor_i,
    motor_d,
    target_speed,
    max_duty,
    servo_p,
    servo_d,
    ramp_speed,
    straight_speed,
    loop_speed,
    cone_num,
/*****rw*****/
    read_only,
    gyro_X,
    gyro_Y,
    gyro_Z,
    acc_X,
    acc_Z,
    acc_Y,
    pitch,
    roll,
    yaw,
    latitude,
    longitude,
    height,
    end_data,
    //..........................
}DATA_NAME;


typedef enum
{
    setspeed=33,
    setangle,
    enable_auto_send,
    motor_brake,
    switch_dir,
    switch_control_mode,
    Gps_read_point,
    Gps_save_point,
    Gps_delete_point,
    Gps_get_saved,
    imu_solve,
    ekf,
    task_switch,
    goto_point,
    auto_update,

}CMD_NAME;//������ö��



typedef struct
{
       uint8 *lora_send_buffer;//���ݷ��ͻ�����
       uint8 *lora_receive_buffer;//���ݽ��ջ�����
       uint16 lora_receive_buffer_index;//���ݽ�������
       struct
       {
           uint8 receive_name;//������
           long receive_data_int;//int�������ݻ�����
           float receive_data_float;//float�������ݻ�����
       }decode;
       struct
       {
               uint8 lora_work_sta : 1;//����ģʽ
               uint8 lora_receive_sta :2;//����״̬��ΪLORA_RECEIVE_STA��ö��
               //uint8 lora_receive_data_sta :1;//���ս��������Ƿ���Ч,0��Ч��1��Ч

       }sta;//lora״̬����
       struct
       {
               void *data_pointer;//����ָ��

               uint8 data_type;//��������


       }data_index[TOTAL_INTERNAL_DATA_SIZE];//�ڲ���������


       struct
       {
               uint8 auto_enable_flag :2;//ʹ�ܱ�־λ
               uint8 index;


       }auto_send;





}LORA_Administrators;//Lora����ṹ��



typedef struct
{
   int int_part;
   float float_part;

}P_T;




extern LORA_Administrators lora;













/********declaration****************/


void    lora_init                   (void);//lora��ʼ��
void    lora_administrators_init    (LORA_Administrators *LA);//Lora����ṹ���ʼ��
void    lora_set_data_index         (LORA_Administrators *LA,DATA_NAME data_n,void *p,DATA_TYPE data_t);//��������ָ������

void    lora_receive_data           (LORA_Administrators *LA);
void    lora_send_data              (LORA_Administrators *LA);
void    lora_send_str               (LORA_Administrators *LA,char* str);//ֱ������ջ���������
void    lora_send_ta                (LORA_Administrators *LA,char* str,uint8 index);
void    encode_lora_str             (LORA_Administrators *LA);//���ݴ������
void    lora_encode_send_data       (LORA_Administrators *LA,DATA_NAME data_name);
void    lora_auto_send              (LORA_Administrators *LA);
void    lora_enable_auto_send       (LORA_Administrators *LA,uint8 enable);


uint8   decode_name                 (LORA_Administrators *LA);
uint8   decode_data                 (LORA_Administrators *LA);


void    decode_lora_str             (LORA_Administrators *LA);
void    cmd_handle                  (LORA_Administrators *LA);//�����
void    dat_handle                  (LORA_Administrators *LA);//���ݷ��ʹ���
void    dac_handle                  (LORA_Administrators *LA);//���ݸı䴦��

void    task_cmd_list               (uint8 index);


void    mysprintf                   (char *string,P_T p);

void    lora_send_double(float x);
void    write_pram_data_flash       (LORA_Administrators *LA);
void    read_pram_data_flash        (LORA_Administrators *LA);




#endif /* CODE_LORA_H_ */
