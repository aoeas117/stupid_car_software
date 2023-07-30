/*
 * imu.h
 *
 *  Created on: 2023Äê1ÔÂ14ÈÕ
 *      Author: ³ËÒÂ¹é
 */

#ifndef CODE_IMU_H_
#define CODE_IMU_H_

#include "zf_common_headfile.h"

#define IMU_PIN_SCL P20_11
#define IMU_PIN_SDA P20_14
#define IMU_IIC_DELAY_TIME 59
#define IMU_TIMEOUT_COUNT 0x00FF

#define IMU_ACC_SAMPLE          (0x20)
#define IMU_GYR_SAMPLE          (0x20)



#define IMU_IIC_ADDR            (0x68)


#define IMU_ACCEL_DATA_XH       (0x1F)
#define IMU_ACCEL_DATA_XL       (0x20)
#define IMU_ACCEL_DATA_YH       (0x21)
#define IMU_ACCEL_DATA_YL       (0x22)
#define IMU_ACCEL_DATA_ZH       (0x23)
#define IMU_ACCEL_DATA_ZL       (0x24)

#define IMU_TEMP_DATAH          (0x1D)
#define IMU_TEMP_DATAL          (0x1E)


#define IMU_GYRO_DATA_XH        (0x25)
#define IMU_GYRO_DATA_XL        (0x26)
#define IMU_GYRO_DATA_YH        (0x27)
#define IMU_GYRO_DATA_YL        (0x28)
#define IMU_GYRO_DATA_ZH        (0x29)
#define IMU_GYRO_DATA_ZL        (0x2A)

#define IMU_XG_ST_DATA          (0x5F)
#define IMU_YG_ST_DATA          (0x60)
#define IMU_ZG_ST_DATA          (0x61)
#define IMU_XA_ST_DATA          (0x3B)
#define IMU_YA_ST_DATA          (0x3C)
#define IMU_ZA_ST_DATA          (0x3D)

#define IMU_DEVICE_CONFIG       (0x11)
#define IMU_DRIVE_CONFIG        (0x13)
#define IMU_INT_CONFIG          (0x14)
#define IMU_INT_CONFIG0         (0x63)
#define IMU_INT_CONFIG1         (0x64)

#define IMU_FIFO_CONFIG         (0x16)
#define IMU_FIFO_CONFIG1        (0x5F)
#define IMU_FIFO_CONFIG2        (0x60)
#define IMU_FIFO_CONFIG3        (0x61)
#define IMU_FIFO_COUNTH         (0x2E)
#define IMU_FIFO_COUNTL         (0x2F)

#define IMU_INTF_CONFIG0        (0x4C)
#define IMU_INTF_CONFIG1        (0x4D)
#define IMU_TMST_CONFIG         (0x54)
#define IMU_FSYNC_CONFIG        (0x62)
#define IMU_PWR_MGMT0           (0x4E)
#define IMU_SELF_TEST_CONFIG    (0x70)
#define IMU_SENSOR_CONFIG0      (0x03)
#define IMU_APEX_CONFIG0        (0x56)
#define IMU_SMD_CONFIG          (0x57)
#define IMU_GYRO_CONFIG0        (0x4F)
#define IMU_GYRO_CONFIG1        (0x51)
#define IMU_ACCEL_CONFIG0       (0x50)
#define IMU_ACCEL_CONFIG1       (0x53)


#define IMU_SIGNAL_PATH_RESET   (0x48)

#define IMU_OFFEST_USER0        (0x77)
#define IMU_OFFEST_USER1        (0x78)
#define IMU_OFFEST_USER2        (0x79)
#define IMU_OFFEST_USER3        (0x7A)
#define IMU_OFFEST_USER4        (0x7B)
#define IMU_OFFEST_USER5        (0x7C)
#define IMU_OFFEST_USER6        (0x7D)
#define IMU_OFFEST_USER7        (0x7E)
#define IMU_OFFEST_USER8        (0x7F)

#define IMU_ACCEL_WOM_X_THR     (0x4A)
#define IMU_ACCEL_WOM_Y_THR     (0x4B)
#define IMU_ACCEL_WOM_Z_THR     (0x4C)






#define IMU_INT_STATUS          (0x2D)


#define IMU_PWR_MGMT0           (0x4E)




#define IMU_WHO_AM_I            (0x75)
#define IMU_ID (0x47)







typedef struct
{


                int16 gyro_x;
                int16 gyro_y;
                int16 gyro_z;
                int16 acc_x;
                int16 acc_y;
                int16 acc_z;


}IMU_Administrators;


extern IMU_Administrators imu;




uint8   imu_init                           (void);


uint8   imu_self_check                     (void);

void    imu_data_update(IMU_Administrators *IA);
float   imu_acc_transition                 (int16 acc_value);
float   imu_gyro_transition                (int16 gyro_value);
void    imu_get_acc                        (void);
void    imu_get_gyro                       (void);



#endif /* CODE_IMU_H_ */
