/*
 * sta_update.h
 *
 *  Created on: 2023Äê4ÔÂ10ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_STA_UPDATE_H_
#define IMU_STA_UPDATE_H_
#include "My_header.h"
#include "data_set.h"

class my_update
{
    public:
        static void carrier_sta_init(IMU_DATA &imu_data,GPS_DATA &gps_data,EKF_STA &pre,EKF_STA &cur);

        static void vel_update( EKF_STA &pre,EKF_STA &cur);

        static void pos_update( EKF_STA &pre,EKF_STA &cur);

        static void att_update( EKF_STA &pre,EKF_STA &cur);

        static void update( EKF_STA &pre,EKF_STA &cur);

        static void data_integrate(IMU_DATA &imu_data,IMU_HANDLED &imu);

        static void data_output(EKF_STA &sta,POS_DATA &pos, VEL_DATA &vel,ATT_DATA &att);

        static void imu_compensate(IMU_HANDLED &imu,IMU_ERROR &imu_error);


};




typedef struct
{
        EKF_STA sta_pre;
        EKF_STA sta_cur;
        IMU_ERROR error;

        unsigned char init_flag :1;

}SOLUTION;

extern SOLUTION update;




#endif /* IMU_STA_UPDATE_H_ */
