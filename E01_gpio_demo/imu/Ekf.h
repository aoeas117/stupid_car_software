/*
 * Ekf.h
 *
 *  Created on: 2023Äê4ÔÂ20ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_EKF_H_
#define IMU_EKF_H_

#include "My_header.h"
#include "data_set.h"

class ekf
{
public:
        static void ekf_update_init(EKF_UPDATE& ekf_update, STA_STD& sta_std, ImuNoise& imu_noise);
        static MatrixXf get_sta_transform_matrix(EKF_STA& presta, IMU_DATA& cur_imu);
        static MatrixXf get_noise_drive_matrix(MatrixXf& a_matrix);
        static void ekf_predict(MatrixXf& phi, MatrixXf& qd, EKF_UPDATE& ekf_update);
        static void ekf_update(MatrixXf& dz, MatrixXf& H, MatrixXf& R, EKF_UPDATE& ekf_update);
        static void ekf_imu_update(EKF_STA& presta, EKF_STA& cursta, EKF_UPDATE& ekf_update, IMU_DATA& cur_imu);
        static void ekf_gps_update(GNSS_DATA& gnss_data, Vector3f l, EKF_STA& presta, EKF_UPDATE& ekf_update);
        static void ekf_feedback(EKF_UPDATE& ekf_update, EKF_STA& cursta);
        static void gnss_data_copy(GPS_DATA &gps_dat,GNSS_DATA &gnss_dat);
};



extern EKF_UPDATE ekf_update;
extern STA_STD ekf_sta_std;
extern ImuNoise imu_noise;
extern Vector3f install_l;
extern GNSS_DATA gnss;

#endif /* IMU_EKF_H_ */
