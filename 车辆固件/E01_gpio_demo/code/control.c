/*
 * control.c
 *
 *  Created on: 2023年4月24日
 *      Author: fuzic
 */


#include "control.h"

CONTROL_Administrators control;
SPEED_PRAM speed_pram={0,0,0};
int control_dire = 0;//改
uint8 line_flag = 0;//改

void CONTROL_Administrators_init(CONTROL_Administrators *CA)
{
    CA->target_point=0;
    CA->last_point=4;

    CA->loop_task.loop_flag=0;
    CA->loop_task.enter_yaw=0;
    CA->loop_task.phase=0;

    CA->sconcer_task.sconcer_flag=0;
    CA->sconcer_task.enter_yaw=0;
    CA->sconcer_task.assist_enter_yaw=0;
    CA->sconcer_task.assist_enter_yaw=0;

    CA->turn_round_task.turn_round_flag=0;
    CA->turn_round_task.enter_yaw=0;
    CA->turn_round_task.phase=0;

    CA->ramp_task.ramp_flag=0;
    CA->ramp_task.phase=0;

    CA->seach_point.flag=0;
    CA->seach_point.target_point=0;
    CA->seach_point.end_point=0;


    CA->search_point_flag=0;
    CA->whole_test=0;



    CA->imu_solution_flag=1;

    CA->distance=0;
    CA->latitude=0;
    CA->longitude=0;



}



void start_search_point(CONTROL_Administrators *CA,uint8 target_point)
{
   target_point--;
   if(target_point==255)
   {
       if(gps_judge_null(&gps,0))
           return;
       CA->seach_point.target_point=0;
       CA->seach_point.end_point=gps_get_end_point(&gps);
   }else
   {
       if(gps_judge_null(&gps,target_point))
           return;
       CA->seach_point.target_point=target_point;
       CA->seach_point.end_point=target_point+1;
   }
   CA->seach_point.flag=1;
   lora_send_str(&lora,"ok!");
}



void search_point_task(void)
{
    if(control.seach_point.flag)
    {
        control.distance=get_two_points_distance(control.latitude,control.longitude,gps.latitude_point[control.seach_point.target_point],gps.longitude_point[control.seach_point.target_point]);
        if(control.distance<0.75)
        {
           control.seach_point.target_point++;
        }
        if(control.seach_point.target_point>=control.seach_point.end_point)
        {

            motor_set_stop(motor,1);
            control.seach_point.flag=0;
            return;
        }
        servo.pid.target_yaw=get_two_points_azimuth(control.latitude,control.longitude,gps.latitude_point[control.seach_point.target_point],gps.longitude_point[control.seach_point.target_point]);//更新目标角

    }


}




void task_of_loop(void)//绕圈任务（方式为惯导闭环）只要保证陀螺仪是准的就可以完成该任务
{


    if(control.loop_task.loop_flag)
    {
            if(control.loop_task.phase==0)//并且到达目标点，并且与赛道方位角偏差不超过+-5
            {
                //设置速度pid参数
                //
                control.loop_task.enter_yaw=servo.pid.current_yaw;//储存好当前的进角
                servo.pid.target_yaw=control.loop_task.enter_yaw-45;
                if(servo.pid.target_yaw<0)
                    servo.pid.target_yaw+=360;
                //可能有参数变化

                control.loop_task.phase=1;//进入阶段1
            }else  if(control.loop_task.phase==1)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-90;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=2;
                }

            }else if(control.loop_task.phase==2)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-135;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=3;
                }
            }else if(control.loop_task.phase==3)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-180;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=4;


                }
            }else if(control.loop_task.phase==4)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-225;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=5;

                }
            }else if(control.loop_task.phase==5)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-270;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=6;

                }

            }else if(control.loop_task.phase==6)
            {
                double error=servo.pid.current_yaw- servo.pid.target_yaw;
                if(error>180)
                error-=360;
                else if(error<-180)
                error+=360;
                if(error<5 && error>-5)
                {
                    servo.pid.target_yaw=control.loop_task.enter_yaw-315;
                    if(servo.pid.target_yaw<0)
                        servo.pid.target_yaw+=360;
                    control.loop_task.phase=7;
                }
            }else if(control.loop_task.phase==7)
            {
                double error=servo.pid.current_yaw-servo.pid.target_yaw;
                if(error>180)
                    error-=360;
                else if(error<-180)
                    error+=360;
                if(error<5 && error>-5)
                {
                    control.loop_task.phase=0;
                    control.loop_task.loop_flag=0;

                    if(control.whole_test)
                    {
                        control.search_point_flag=1;//开启寻点
                        control.target_point++;//寻找下一个点
                    }
               }
            }
      }

}





//void sconcer_task(void)
//{
//    if(control.sconcer_task.sconcer_flag==1)
//    {
//
//        if(control.sconcer_task.phase==0)//并且到达目标点，并且与赛道方位角偏差不超过+-5
//        {
//            //设置速度pid参数
//
//            //
//            control.sconcer_task.enter_yaw=servo.pid.current_yaw;
//            servo.pid.target_yaw=control.sconcer_task.enter_yaw-LEAN_YAW;
//            if(servo.pid.target_yaw>360)
//                servo.pid.target_yaw-=360;
//
//            control.sconcer_task.distance_count=0;
//            control.sconcer_task.block_count=1;
//            control.sconcer_task.phase=1;
//        }
//
//        if(control.sconcer_task.distance_count>2.3)//大于1.5m
//        {
//            control.sconcer_task.distance_count=0;
//            control.sconcer_task.block_count++;
//
//            if(control.sconcer_task.block_count==BLOCK_COUNT)
//            {
//                servo.pid.target_yaw=control.sconcer_task.enter_yaw;
//                control.sconcer_task.sconcer_flag=0;
//                control.sconcer_task.phase=0;
//
//                if(control.whole_test)
//                {
//
//                    if(control.whole_test)
//                    {
//                        control.search_point_flag=1;//开启寻点
//                        control.target_point++;//寻找下一个点
//                    }
//                }
//
//                return;
//            }
//            if(control.sconcer_task.phase==1)
//            {
//               servo.pid.target_yaw=control.sconcer_task.enter_yaw+LEAN_YAW;
//               if(servo.pid.target_yaw<0)
//                   servo.pid.target_yaw+=360;
//               control.sconcer_task.phase=2;
//
//            }else  if(control.sconcer_task.phase==2)
//            {
//               servo.pid.target_yaw=control.sconcer_task.enter_yaw-LEAN_YAW;
//               if(servo.pid.target_yaw>360)
//                   servo.pid.target_yaw-=360;
//               control.sconcer_task.phase=1;
//
//            }
//
//        }
//
//    }
//
//}
void sconcer_task(void)
{

}








void turn_round_task(void)
{





}


void ramp_task(void)
{

}



//方向（目标角更新（目标点更新，当前位置更新），姿态角更新）
void direacion_control(CONTROL_Administrators *CA)
{

        //（条件判断）位置更新（CA->latitude）、（CA->longitude）

      CA->latitude=RAD_TO_ANGLE(location_licheng.Latitude);
      CA->longitude=RAD_TO_ANGLE(location_licheng.Longitude);
        //目标点更新
      if(CA->search_point_flag)
      {
          control.distance=get_two_points_distance(control.latitude,control.longitude,gps.latitude_point[control.target_point],gps.longitude_point[control.target_point]);
          point_pram_control(CA);
          double azimuth=get_two_points_azimuth(control.latitude,control.longitude,gps.latitude_point[control.target_point],gps.longitude_point[control.target_point]);
          double temp=azimuth-servo.pid.target_yaw;
          temp=temp>0? temp:-temp;
          if(temp>180)
              temp = 360 - temp;
          if(temp>30&&line_flag==1)//改，相差过大，提前结束直线
          {
              line_flag = 0;
              control_dire = 1;
          }

          if(control_dire)//改
          {
          if(sin(ANGLE_TO_RAD(temp))*control.distance>0.3&&control.distance<=1)
              servo.pid.target_yaw=azimuth;
          else if(control.distance>1)
              servo.pid.target_yaw=azimuth;
          if(line_flag == 1)
          {
              control_dire = 0;//如果直线运动，置一次方向后disable
          }
          }


      }

    task_of_loop();//绕圈
    sconcer_task();//s弯
    turn_round_task();//掉头
    ramp_task();//坡道
    search_point_task();//寻点

}




void point_pram_control(CONTROL_Administrators *CA)//点位参数控制
{

    if(control.distance<1)
    {
         if(control.target_point==RAMP_DOWN_SPEED)
         {

         }
         else if(control.target_point==RAMP_POINT)//坡道点
         {



         }else if(control.target_point==TURN_ROUND_POINT)//返回点
         {



         }else if(control.target_point==LOOP_POINT)//绕环点
         {
             CA->loop_task.loop_flag=1;//开启绕圈
             CA->search_point_flag=0;//关闭寻点
             servo.pid.p=4;
             return;


         }else if(control.target_point==SCONCER_POINT)//s弯点
         {



         }else if(control.target_point==SCONCER_POINT+speed_pram.cone_num)//最后一个点
         {
             motor_set_stop(motor,1);//刹车
             control.target_point=0;
             CA->search_point_flag=0;//关闭寻点

             motor->control_mode=1;
             control.imu_solution_flag=0;
             pwm_set_duty(SERVO_PWM_CH,SERVO_MID);

             return;
         }

         control.target_point++;

     }else if(control.distance<(3+(float)motor->motor_speed.target_speed/400))
     {
         if(control.target_point==RAMP_DOWN_SPEED)
         {
             motor->motor_speed.target_speed=speed_pram.ramp_speed;
             servo.pid.p=2;
         }
         else if(control.target_point==RAMP_POINT)//坡道点
         {


         }else if(control.target_point==TURN_ROUND_POINT)//返回点
         {


         }else if(control.target_point==LOOP_POINT)//绕环点
         {
             motor->motor_speed.target_speed=speed_pram.loop_speed;


         }else if(control.target_point==SCONCER_POINT)//s弯点
         {
             motor->motor_speed.target_speed=BEND_SPEED;
             servo.pid.p=7;

         }else if(control.target_point==SCONCER_POINT+speed_pram.cone_num)//最后一个点
         {

         }
     }else if(control.distance>3)
     {

         if(control.target_point==RAMP_DOWN_SPEED)
          {
             motor->motor_speed.target_speed=speed_pram.straight_speed;
             servo.pid.p=7;


          }
          else if(control.target_point==RAMP_POINT)//坡道点
          {


          }else if(control.target_point==TURN_ROUND_POINT)//返回点
          {
              motor->motor_speed.target_speed=speed_pram.straight_speed;
              servo.pid.p=5;

          }else if(control.target_point==LOOP_POINT)//绕环点
          {

          }else if(control.target_point==SCONCER_POINT)//s弯点
          {
              motor->motor_speed.target_speed=speed_pram.straight_speed;

          }else if(control.target_point==SCONCER_POINT+speed_pram.cone_num)//最后一个点
          {
             motor->motor_speed.target_speed=speed_pram.straight_speed;
             servo.pid.p=5;
          }







     }
     if(control.target_point == 0)//改 ，第一个点跑直线,接近寻点
     {
         if(control.distance <=10)
         {
             control_dire = 1;
             line_flag = 0;//撤销直线行驶
         }
     }




}







