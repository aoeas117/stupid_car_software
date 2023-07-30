/*
 * data_set.c
 *
 *  Created on: 2023Äê3ÔÂ31ÈÕ
 *      Author: fuzic
 */


#include "data_set.h"




IMU_DATA imu_data;
GPS_DATA gps_data;
POS_DATA pos_data;
VEL_DATA vel_data;
ATT_DATA att_data;
IMU_DATA imu_bias;
unsigned char gps_asistant_flag=0;
float bridge=0;
unsigned char gps_asistant_time_count=0;
unsigned char gps_asistant_yaw_flag=0;



void data_set_init(void)
{

    imu_data.update_flag=0;
    gps_data.update_flag=0;
    imu_bias.update_flag=0;

}
















