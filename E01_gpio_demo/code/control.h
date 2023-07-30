/*
 * control.h
 *
 *  Created on: 2023年4月24日
 *      Author: fuzic
 */

#ifndef CODE_CONTROL_H_
#define CODE_CONTROL_H_


#include "zf_common_headfile.h"

#define LOW_DISTANCE 1
#define FAST_DISTANCE 6

#define RAMP_DOWN_SPEED 0//坡道减速点
#define RAMP_POINT 1//坡道点
#define TURN_ROUND_POINT 2 //返回点
#define LOOP_POINT  3
#define SCONCER_POINT 4
#define END_POINT 8


#define BLOCK_COUNT 4
#define LEAN_YAW 30
#define STRAIGHT_SPEED 2500//直道速度
#define RAMP_SPEED 1500//坡道速度
#define LOOP_SPEED 1500//圆环速度
#define BEND_SPEED 1200//弯道速度


typedef struct
{
        uint8 target_point;//目标点
        uint8 last_point;//最后一个点
        double distance;//距离
        double yaw;//航向角（最终结果）
        double latitude;
        double longitude;
        uint8 search_point_flag :1;
        uint8 whole_test :1;

        uint8 imu_solution_flag :1;

        struct
        {
                uint8 loop_flag :1;//任务执行标志
                uint8 phase :4;
                double enter_yaw;//进角
        }loop_task;

        struct
        {
                uint8 sconcer_flag :1;//s弯标志
                uint8 phase :4;//阶段标志
                uint8 block_count;//过障数量
                double enter_yaw;//进角
                double assist_enter_yaw;//辅助进角
                float distance_count;//计程



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
void point_pram_control(CONTROL_Administrators *CA);//点位参数控制
void start_search_point(CONTROL_Administrators *CA,uint8 target_point);
void task_of_loop(void);
void sconcer_task(void);
void turn_round_task(void);
void ramp_task(void);

#endif /* CODE_CONTROL_H_ */
