/*
 * imu_types.h
 *
 *  Created on: 2023��4��10��
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
};//��̬



struct CARRIER_STA
{
    Vector3d pos;
    Vector3f vel;
    ATTITUDE att;
};//����״̬

typedef struct
{
    Vector3f dvel;
    Vector3f dtheta;
}IMU_HANDLED;//imu������ֺ������




typedef struct
{
    Vector3f pos;
    Vector3f error;

}GNSS_DATA;


typedef struct

{
    Vector3f gyrbias;//��������ƫ���
    Vector3f accbias;//���ٶȼ���ƪ���
    Vector3f gyrscale;//�����Ǳ���ϵ�����
    Vector3f accscale;//���ٶȼƱ���ϵ�����


}IMU_ERROR;

typedef struct
{
    Vector3f pos;
    Vector3f vel;
    Vector3f euler;
    Vector3f gyrbias;//��������ƫ���
    Vector3f accbias;//���ٶȼ���ƪ���
    Vector3f gyrscale;//�����Ǳ���ϵ�����
    Vector3f accscale;//���ٶȼƱ���ϵ�����

}STA_STD;//��׼��

typedef struct  {
    Vector3f gyr_arw;//�����ǽǶ��������
    Vector3f acc_vrw;//���ٶȼ��ٶ��������
    Vector3f gyrbias_std;//��������ƫ��׼��
    Vector3f accbias_std;//���ٶȼ���ƫ��׼��
    Vector3f gyrscale_std;//�����Ǳ������ӱ�׼��
    Vector3f accscale_std;//���ٶȼƱ������ӱ�׼��
} ImuNoise;


typedef struct
{
    Vector3d pos;
    Vector3f vel;
    ATTITUDE att;
    IMU_HANDLED imu;
}EKF_STA;//״̬

struct EKF_UPDATE
{
    EKF_UPDATE():dx(21,1),cov(21,21),qc(18,18){}
    MatrixXf dx;//update(״̬����)
    MatrixXf cov;//update()
    MatrixXf qc;//����
    IMU_ERROR imu_error;

};











#endif /* IMU_IMU_TYPES_H_ */
