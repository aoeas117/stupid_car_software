/*
 * t.cpp
 *
 *  Created on: 2023年4月2日
 *      Author: fuzic
 */


#include "My_header.h"

#include <cpp_api.h>


#ifdef __cplusplus
extern "C" {
#endif





void update_data_init(void)
{
    update.init_flag=0;
    update.sta_pre.imu.dtheta.set_Zeros();
    update.sta_pre.imu.dvel.set_Zeros();

    ekf_update.imu_error.gyrbias[0]=-0.07367,  ekf_update.imu_error.gyrbias[1]=-0.013835,  ekf_update.imu_error.gyrbias[2]=0.012771;//rad/s
    ekf_update.imu_error.accbias[0]=0.898806,  ekf_update.imu_error.accbias[1]=-0.8214425,  ekf_update.imu_error.accbias[2]=0.142;//m/s2
    ekf_update.imu_error.gyrscale[0]=0.002, ekf_update.imu_error.gyrscale[1]=0.002, ekf_update.imu_error.gyrscale[2]=0.002;
    ekf_update.imu_error.accscale[0]=0.01, ekf_update.imu_error.accscale[1]=0.01, ekf_update.imu_error.accscale[2]=0.01;


    install_l[0]=0,install_l[1]=0,install_l[2]=0;//安装参数


    ekf_sta_std.vel[0]=0.6046,       ekf_sta_std.vel[1]=0.637912,       ekf_sta_std.vel[2]=1.479595;
    ekf_sta_std.pos[0]=0.0,       ekf_sta_std.pos[1]=0.0,       ekf_sta_std.pos[2]=1;
    ekf_sta_std.euler[0]=0.233574,     ekf_sta_std.euler[1]=0.173277,     ekf_sta_std.euler[2]=0.197946;
    ekf_sta_std.accbias[0]=0.006544,   ekf_sta_std.accbias[1]=0.0065,   ekf_sta_std.accbias[2]=0.006658;
    ekf_sta_std.accscale[0]=0,  ekf_sta_std.accscale[1]=0,  ekf_sta_std.accscale[2]=0;
    ekf_sta_std.gyrbias[0]=0.001236,   ekf_sta_std.gyrbias[1]=0.0012,   ekf_sta_std.gyrbias[2]=0.0012;
    ekf_sta_std.gyrscale[0]=0.0,  ekf_sta_std.gyrscale[1]=0.0,  ekf_sta_std.gyrscale[2]=0.0;

    imu_noise.acc_vrw[0]=0.000637,         imu_noise.acc_vrw[1]=0.000637,         imu_noise.acc_vrw[2]=0.000686;
    imu_noise.gyr_arw[0]=0.0028*(PI/180.0),         imu_noise.gyr_arw[1]=0.0028*(PI/180.0),         imu_noise.gyr_arw[2]=0.0028*(PI/180.0);
    imu_noise.gyrbias_std[0]=0.0001,     imu_noise.gyrbias_std[1]=0.0001,     imu_noise.gyrbias_std[0]=0.001;
    imu_noise.accbias_std[0]=0.008,     imu_noise.accbias_std[1]=0.008,     imu_noise.accbias_std[2]=0.008;
    imu_noise.gyrscale_std[0]=0,    imu_noise.gyrscale_std[1]=0,    imu_noise.gyrscale_std[2]=0;
    imu_noise.accscale_std[0]=0,    imu_noise.accscale_std[1]=0,    imu_noise.accscale_std[2]=0;


    ekf::ekf_update_init(ekf_update,ekf_sta_std,imu_noise);//ekf初始状态矩阵


}




void get_gps_yaw(void)
{


    if(!gps_asistant_flag)
    {
        if(gps_data.speed>2)
        {

            update.sta_pre.att.a_euler.set_Zeros();
            update.sta_pre.att.a_euler[2]=(gps_data.yaw)*3.1415926/180;

            update.sta_pre.att.a_matrix = Rotation::euler2matrix(update.sta_pre.att.a_euler);
            update.sta_pre.att.a_quaterniond = Rotation::euler2quaternion(update.sta_pre.att.a_euler);
            update.sta_cur=update.sta_pre;

            gps_asistant_flag=1;
        }
    }
}


void gps_yaw_asistant(void)
{

    if(gps_asistant_yaw_flag==1)
    {
        if(gps_data.speed>3&&gps_asistant_flag)
        {
            float error=(gps_data.yaw)*3.1415926/180-att_data.yaw;
            if(error<3 && error>-3)
            {
                update.sta_pre.att.a_euler.set_Zeros();
                update.sta_pre.att.a_euler[2]=(gps_data.yaw)*3.1415926/180;
                update.sta_pre.att.a_matrix = Rotation::euler2matrix(update.sta_pre.att.a_euler);
                update.sta_pre.att.a_quaterniond = Rotation::euler2quaternion(update.sta_pre.att.a_euler);
                update.sta_cur=update.sta_pre;
            }
       }
       gps_asistant_yaw_flag=0;
       extern int control_dire,line_flag;//改
       control_dire = 1;//改
       line_flag = 1;//改
  }
}



void imu_sta_init(void)
{
    update.sta_pre.vel.set_Zeros();

    update.sta_pre.pos[0] = (3.1415926535 / 180.0) * gps_data.latitude;
   update.sta_pre.pos[1] = (3.1415926535 / 180.0) * gps_data.longitude;
   update.sta_pre.pos[2] = gps_data.height;

    update.sta_pre.att.a_euler.set_Zeros();
    update.sta_pre.att.a_euler[2]=PI;

    update.sta_pre.att.a_matrix = Rotation::euler2matrix(update.sta_pre.att.a_euler);
    update.sta_pre.att.a_quaterniond = Rotation::euler2quaternion(update.sta_pre.att.a_euler);
    update.sta_cur=update.sta_pre;

}




void  imu_solution(void)
{


   if(imu_data.update_flag==1)
   {
   if(update.init_flag==0)
        {
            //my_update::carrier_sta_init(imu_data, gps_data,update.sta_pre, update.sta_cur);//获取初始位置、速度、姿态

            //速度
            update.sta_pre.vel.set_Zeros();


            //位置
           update.sta_pre.pos[0] = (3.1415926535 / 180.0) * gps_data.latitude;
           update.sta_pre.pos[1] = (3.1415926535 / 180.0) * gps_data.longitude;
           update.sta_pre.pos[2] = gps_data.height;

           //姿态
           update.sta_pre.att.a_euler.set_Zeros();
           update.sta_pre.att.a_euler[2]=PI;
           update.sta_pre.att.a_matrix = Rotation::euler2matrix(update.sta_pre.att.a_euler);
           update.sta_pre.att.a_quaterniond = Rotation::euler2quaternion(update.sta_pre.att.a_euler);

           //零漂误差
            ekf_update.imu_error.gyrbias[0]=imu_bias.gyro_x,  ekf_update.imu_error.gyrbias[1]=imu_bias.gyro_y,  ekf_update.imu_error.gyrbias[2]=imu_bias.gyro_z;//rad/s
            ekf_update.imu_error.accbias[0]=imu_bias.acc_x,  ekf_update.imu_error.accbias[1]=imu_bias.acc_y,  ekf_update.imu_error.accbias[2]=9.7803267715+imu_bias.acc_z;//m/s2

            update.sta_cur=update.sta_pre;

            update.init_flag=1;
        }else if(update.init_flag==1)
        {

            my_update::data_integrate(imu_data,update.sta_cur.imu);//对惯导数据积分

            ekf::ekf_imu_update(update.sta_pre, update.sta_cur,ekf_update,imu_data);//拓展卡尔曼滤波

            if(gps_data.update_flag==1)//gps数据用作观测向量
            {
//                ekf::gnss_data_copy(gps_data,gnss);//将gps数据copy进gnss
//                ekf::ekf_gps_update(gnss,install_l,update.sta_pre,ekf_update);
//               ekf::ekf_feedback(ekf_update,update.sta_cur);//补偿att、pos、vel、误差get_gps_yaw
                //gps_yaw_asistant();
                get_gps_yaw();
                gps_data.update_flag=0;
            }
            my_update::data_output(update.sta_cur,pos_data, vel_data, att_data);//输出数据
            update.sta_pre=update.sta_cur;

//            bridge=1;
    }

       imu_data.update_flag=0;
   }

}




#ifdef __cplusplus
}
#endif












