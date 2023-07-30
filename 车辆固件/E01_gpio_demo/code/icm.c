/*
 * icm.c
 *
 *  Created on: 2023年3月16日
 *      Author: 乘衣归
 */


#include "icm.h"






ICM_Administrators icm;








void icm_init(void)
{
    icm.filter.filter_enable=1;
    icm.filter.filter_count=0;;

//api接口提供不同的imu驱动
    icm.init=imu_init;
    icm.get_acc=imu_get_acc;
    icm.get_gyro=imu_get_gyro;
//
    icm.init();


}



void icm_get_data(void)
{
    icm.get_acc();
    icm.get_gyro();
//imu数据读取接口
    imu_data.acc_x=imu_acc_transition(imu.acc_y)*9.7803267715;  //m2/s
    imu_data.acc_y=imu_acc_transition(imu.acc_x)*9.7803267715;
    imu_data.acc_z=-imu_acc_transition(imu.acc_z)*9.7803267715;

   imu_data.gyro_x=ANGLE_TO_RAD(imu_gyro_transition(imu.gyro_y));//弧度
   imu_data.gyro_y=ANGLE_TO_RAD(imu_gyro_transition(imu.gyro_x));
   imu_data.gyro_z=-ANGLE_TO_RAD(imu_gyro_transition(imu.gyro_z));
//

    imu_data.update_flag=1;
}


void icm_filter(void)
{
    if(icm.filter.filter_enable==1)
    {
        if( imu_data.update_flag==1)
        {
            icm.filter.acc_x+= imu_data.acc_x;
            icm.filter.acc_y+= imu_data.acc_y;
            icm.filter.acc_z+= imu_data.acc_z;
            icm.filter.gyro_x+=imu_data.gyro_x;
            icm.filter.gyro_y+=imu_data.gyro_y;
            icm.filter.gyro_z+=imu_data.gyro_z;

            icm.filter.filter_count++;
            imu_data.update_flag=0;
        }
        if(icm.filter.filter_count==ICM_FILTER_TIMES)
        {
            imu_bias.acc_x= icm.filter.acc_x/(float)icm.filter.filter_count;
            imu_bias.acc_y= icm.filter.acc_y/(float)icm.filter.filter_count;
            imu_bias.acc_z= icm.filter.acc_z/(float)icm.filter.filter_count;
            imu_bias.gyro_x= icm.filter.gyro_x/(float)icm.filter.filter_count;
            imu_bias.gyro_y= icm.filter.gyro_y/(float)icm.filter.filter_count;
            imu_bias.gyro_z= icm.filter.gyro_z/(float)icm.filter.filter_count;
            imu_bias.update_flag=1;
            icm.filter.filter_count=0;
            icm.filter.filter_enable=0;
        }
    }
}


void icm_filter_enable(void)
{
    icm.filter.filter_enable=1;
}








