#ifndef _mw1208_h
#define _mw1208_h

#include "common.h"
#include "zf_uart.h"
#include "gui.h"

#define pin_mw1208_aux D4
#define pin_mw1208_md0 D5
#define pin_mw1208_tx UART2_TX_A02
#define pin_mw1208_rx UART2_RX_A03
#define mw1208_uart UART_2
#define mw1208_baud 115200
#define DATA_SIZE 32


typedef struct
{
	uint32 current_time;
	uint8 Flag;
}CHECK;








typedef enum 
{
	data_receive_start=0,//开始接收数组
	data_recevie_unfinish,//结束接收数组&&待使用数据
	data_receive_finish,//数组使用完成

}_RECEIVE_DATA_STA;




/***********数据类型*************/
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
}DATA_TYPE;





/**************变量名*************/
typedef enum
{
	void_data=32,
	motor_pwm,
	motor_p,
	motor_ip,
	motor_d,
	target_speed,
	max_duty,
	servo_p,
	servo_d,
/*****以上为rw*****/
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

	//..........................
}DATA_NAME;

/********************************/


typedef enum
{
	setspeed=33,
	setangle,
	enable_auto_send,
	motor_brake,
	switch_dir,
	switch_control_mode,
	gps_get_point,
	gps_save_point,
	gps_delete_point,
	Gps_get_saved,
	imu_solve,
	ekf,
	task_switch,
	goto_point,
	auto_update,

}CMD_NAME;


extern CHECK check;

extern uint8 lora_receive_data[DATA_SIZE];
extern char lora_send_data[DATA_SIZE];//发送缓存数组
extern DATA_NAME data_name;

void mw1208_init					(void);
void mw1208_set						(void);//配置mw1208
void mw1208_uart_handle		();
void mw1208_send_data			(char *str);

void encode_lora_cmd			(CMD_NAME cmd,void *data,DATA_TYPE data_type);
void encode_lora_data			(DATA_NAME data_name,void *data,DATA_TYPE data_type);
void dat_handle						(u8 *lora_str);
void decode_lora_str			(u8 *lora_str);
void decode_data					(u8 *lora_str,char *string);
u8 	 decode_name					(char* string);



void cut_str							(char*string);


#endif
