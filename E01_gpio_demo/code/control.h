/*
 * control.h
 *
 *  Created on: 2023��4��24��
 *      Author: fuzic
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_


#include "zf_common_headfile.h"

#define LOW_DISTANCE 1
#define FAST_DISTANCE 6

#define RAMP_DOWN_SPEED 0//�µ����ٵ�
#define RAMP_POINT 1//�µ���
#define TURN_ROUND_POINT 2 //���ص�
#define LOOP_POINT  3
#define SCONCER_POINT 4
#define END_POINT 8


#define BLOCK_COUNT 4
#define LEAN_YAW 30
#define STRAIGHT_SPEED 2500//ֱ���ٶ�
#define RAMP_SPEED 1500//�µ��ٶ�
#define LOOP_SPEED 1500//Բ���ٶ�
#define BEND_SPEED 1200//����ٶ�


typedef struct
{
        uint8 target_point;//Ŀ���
        uint8 last_point;//���һ����
        double distance;//����
        double yaw;//����ǣ����ս����
        double latitude;
        double longitude;
        uint8 search_point_flag :1;
        uint8 whole_test :1;

        uint8 imu_solution_flag :1;

        struct
        {
                uint8 loop_flag :1;//����ִ�б�־
                uint8 phase :4;
                double enter_yaw;//����
        }loop_task;

        struct
        {
                uint8 sconcer_flag :1;//s���־
                uint8 phase :4;//�׶α�־
                uint8 block_count;//��������
                double enter_yaw;//����
                double assist_enter_yaw;//��������
                float distance_count;//�Ƴ�



        }sconcer_task;

        struct
        {
                uint8 ramp_flag :1;
                uint8 phase;


        }ramp_task;


        struct
        {
                uint8 turn_round_flag :1;
                uint8 phase :3;
                double enter_yaw;
        }turn_round_task;

        struct
        {
                uint8 flag:1;
                uint8 target_point:7;
                uint8 end_point :6;


        }seach_point;






}CONTROL_Administrators;




typedef struct
{
        uint16 ramp_speed;
        uint16 straight_speed;
        uint16 loop_speed;
        uint8 cone_num;
}SPEED_PRAM;

extern SPEED_PRAM speed_pram;








extern CONTROL_Administrators control;


void CONTROL_Administrators_init(CONTROL_Administrators *CA);
void direacion_control(CONTROL_Administrators *CA);
void point_pram_control(CONTROL_Administrators *CA);//��λ��������
void start_search_point(CONTROL_Administrators *CA,uint8 target_point);
void task_of_loop(void);
void sconcer_task(void);
void turn_round_task(void);
void ramp_task(void);

#endif /* CODE_CONTROL_H_ */
