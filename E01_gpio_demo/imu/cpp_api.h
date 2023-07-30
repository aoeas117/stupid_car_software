/*
 * t.h
 *
 *  Created on: 2023Äê4ÔÂ2ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_CPP_API_H_
#define IMU_CPP_API_H_




#ifdef __cplusplus
extern "C" {
#endif


void    update_data_init    (void);
void    imu_solution        (void);
void    gps_yaw_asistant    (void);
void    imu_sta_init        (void);
void    get_gps_yaw         (void);

#ifdef __cplusplus
}
#endif




#endif /* IMU_CPP_API_H_ */
