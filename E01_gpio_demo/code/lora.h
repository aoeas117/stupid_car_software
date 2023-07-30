/*
 * lora.h
 *
 *  Created on: 2023年1月13日
 *      Author: 乘衣归
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
}DATA_TYPE;//数据类型枚举


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

}CMD_NAME;//命令名枚举



typedef struct
{
       uint8 *lora_send_buffer;//数据发送缓存区
       uint8 *lora_receive_buffer;//数据接收缓存区
       uint16 lora_receive_buffer_index;//数据接收索引
       struct
       {
           uint8 receive_name;//解析名
           long receive_data_int;//int解析数据缓存区
           float receive_data_float;//float解析数据缓存区
       }decode;
       struct
       {
               uint8 lora_work_sta : 1;//工作模式
               uint8 lora_receive_sta :2;//接收状态，为LORA_RECEIVE_STA的枚举
               //uint8 lora_receive_data_sta :1;//接收解析数据是否有效,0无效，1有效

       }sta;//lora状态管理
       struct
       {
               void *data_pointer;//数据指针

               uint8 data_type;//数据类型


       }data_index[TOTAL_INTERNAL_DATA_SIZE];//内部数据索引


       struct
       {
               uint8 auto_enable_flag :2;//使能标志位
               uint8 index;


       }auto_send;





}LORA_Administrators;//Lora管理结构体



typedef struct
{
   int int_part;
   float float_part;

}P_T;




extern LORA_Administrators lora;













/********declaration****************/


void    lora_init                   (void);//lora初始化
void    lora_administrators_init    (LORA_Administrators *LA);//Lora管理结构体初始化
void    lora_set_data_index         (LORA_Administrators *LA,DATA_NAME data_n,void *p,DATA_TYPE data_t);//设置数据指针索引

void    lora_receive_data           (LORA_Administrators *LA);
void    lora_send_data              (LORA_Administrators *LA);
void    lora_send_str               (LORA_Administrators *LA,char* str);//直接向接收机发送数据
void    lora_send_ta                (LORA_Administrators *LA,char* str,uint8 index);
void    encode_lora_str             (LORA_Administrators *LA);//数据传输编码
void    lora_encode_send_data       (LORA_Administrators *LA,DATA_NAME data_name);
void    lora_auto_send              (LORA_Administrators *LA);
void    lora_enable_auto_send       (LORA_Administrators *LA,uint8 enable);


uint8   decode_name                 (LORA_Administrators *LA);
uint8   decode_data                 (LORA_Administrators *LA);


void    decode_lora_str             (LORA_Administrators *LA);
void    cmd_handle                  (LORA_Administrators *LA);//命令处理
void    dat_handle                  (LORA_Administrators *LA);//数据发送处理
void    dac_handle                  (LORA_Administrators *LA);//数据改变处理

void    task_cmd_list               (uint8 index);


void    mysprintf                   (char *string,P_T p);

void    lora_send_double(float x);
void    write_pram_data_flash       (LORA_Administrators *LA);
void    read_pram_data_flash        (LORA_Administrators *LA);




#endif /* CODE_LORA_H_ */
