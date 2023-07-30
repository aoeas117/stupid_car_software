/*
 * imu_types.h
 *
 *  Created on: 2023年4月10日
 *      Author: fuzic
 */

#ifndef IMU_IMU_TYPES_H_
#define IMU_IMU_TYPES_H_
#include "My_header.h"


struct ATTITUDE
{
    ATTITUDE():a_matrix(3,3){}
    Vector3f a_euler;
    MatrixXf a_matrix;
    Quaterniond a_quaterniond;
};//姿态



struct CARRIER_STA
{
    Vector3d pos;
    Vector3f vel;
    ATTITUDE att;
};//载体状态

typedef struct
{
    Vector3f dvel;
    Vector3f dtheta;
}IMU_HANDLED;//imu输出积分后的数据




typedef struct
{
    Vector3f pos;
    Vector3f error;

}GNSS_DATA;


typedef struct

{
    Vector3f gyrbias;//陀螺仪零偏误差
    Vector3f accbias;//加速度计零篇误差
    Vector3f gyrscale;//陀螺仪比例系数误差
    Vector3f accscale;//加速度计比例系数误差


}IMU_ERROR;

typedef struct
{
    Vector3f pos;
    Vector3f vel;
    Vector3f euler;
    Vector3f gyrbias;//陀螺仪零偏误差
    Vector3f accbias;//加速度计零篇误差
    Vector3f gyrscale;//陀螺仪比例系数误差
    Vector3f accscale;//加速度计比例系数误差

}STA_STD;//标准差

typedef struct  {
    Vector3f gyr_arw;//陀螺仪角度随机游走
    Vector3f acc_vrw;//加速度计速度随机游走
    Vector3f gyrbias_std;//陀螺仪零偏标准差
    Vector3f accbias_std;//加速度计零偏标准差
    Vector3f gyrscale_std;//陀螺仪比例因子标准差
    Vector3f accscale_std;//加速度计比例因子标准差
} ImuNoise;


typedef struct
{
    Vector3d pos;
    Vector3f vel;
    ATTITUDE att;
    IMU_HANDLED imu;
}EKF_STA;//状态

struct EKF_UPDATE
{
    EKF_UPDATE():dx(21,1),cov(21,21),qc(18,18){}
    MatrixXf dx;//update(状态矩阵)
    MatrixXf cov;//update()
    MatrixXf qc;//不变
    IMU_ERROR imu_error;

};











#endif /* IMU_IMU_TYPES_H_ */
