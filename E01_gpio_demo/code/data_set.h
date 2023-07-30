/*
 * data_set.h
 *
 *  Created on: 2023Äê3ÔÂ31ÈÕ
 *      Author: fuzic
 */

#ifndef CODE_DATA_SET_H_
#define CODE_DATA_SET_H_



typedef struct
{
        float gyro_x;
        float gyro_y;
        float gyro_z;
        float acc_x;
        float acc_y;
        float acc_z;
        unsigned char update_flag;

}IMU_DATA;

typedef struct
{
        float latitude;
        float longitude;
        float height;

        float yaw;
        float speed;

        float hoop_x;
        float hoop_y;
        float hoop_z;

        unsigned char update_flag;
}GPS_DATA;


typedef struct
{
        float latitude;
        float longitude;
        float height;
}POS_DATA;



typedef struct
{
        float vel_x;
        float vel_y;
        float vel_z;
}VEL_DATA;


typedef struct
{
        float roll;
        float pitch;
        float yaw;


}ATT_DATA;



extern unsigned char gps_asistant_flag;


//input
extern IMU_DATA imu_data;
extern GPS_DATA gps_data;
//output
extern POS_DATA pos_data;
extern VEL_DATA vel_data;
extern ATT_DATA att_data;
extern IMU_DATA imu_bias;
extern float bridge;
extern unsigned char gps_asistant_time_count;
extern unsigned char gps_asistant_yaw_flag;




void data_set_init(void);


#endif /* CODE_DATA_SET_H_ */
