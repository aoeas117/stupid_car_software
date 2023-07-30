/*
 * imu.c
 *
 *  Created on: 2023��1��14��
 *      Author: ���¹�
 */

#include "imu.h"



IMU_Administrators imu={0};
static soft_iic_info_struct imu_iic_info_struct;;



#define imu_write_register(reg, data)      (soft_iic_write_8bit_register(&imu_iic_info_struct, (reg), (data)))
#define imu_read_register(reg)             (soft_iic_read_8bit_register(&imu_iic_info_struct, (reg)))
#define imu_read_registers(reg, data, len) (soft_iic_read_8bit_registers(&imu_iic_info_struct, (reg), (data), (len)))



uint8 imu_init(void)//δ���
{
    uint8 return_state=0;
    system_delay_ms(10);
    soft_iic_init(&imu_iic_info_struct, IMU_IIC_ADDR, IMU_IIC_DELAY_TIME, IMU_PIN_SCL, IMU_PIN_SDA);
    if(imu_self_check())
    {
        lora_send_str(&lora,"check error");
        return_state = 1;
    }

    imu_write_register(IMU_DEVICE_CONFIG,       0x01);                 //softreset
    system_delay_ms(2);
                                                                       //
    imu_write_register(IMU_PWR_MGMT0,           0x0f);                 // ���������Ǻͼ��ٶȼ�

    imu_write_register(IMU_GYRO_CONFIG0,        0x26);  // ��2000 dps  1khz
    imu_write_register(IMU_ACCEL_CONFIG0,       0x26);  // ��8g        1khz



    return return_state;
}










uint8 imu_self_check (void)
{
    uint8 dat = 0, return_state = 0;
    uint16 timeout_count = 0;

    while(IMU_ID!= dat)                                                          // �ж� ID �Ƿ���ȷ
    {
        if(timeout_count ++ > IMU_TIMEOUT_COUNT)
        {
            return_state =  1;
            break;
        }
        dat = imu_read_register(IMU_WHO_AM_I);
        system_delay_ms(10);
    }

    return return_state;
}


float imu_acc_transition (int16 acc_value)
{
    float acc_data = 0;
    switch(IMU_ACC_SAMPLE)
    {
        case 0x60: acc_data = (float)acc_value / 16384; break;      // ���ٶȼ�����Ϊ:��2g          ��ȡ���ļ��ٶȼ����� ����16384      ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x40: acc_data = (float)acc_value / 8192;  break;      // ���ٶȼ�����Ϊ:��4g          ��ȡ���ļ��ٶȼ����� ����8192       ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x20: acc_data = (float)acc_value / 4096;  break;      // ���ٶȼ�����Ϊ:��8g          ��ȡ���ļ��ٶȼ����� ����4096       ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        case 0x00: acc_data = (float)acc_value / 2048;  break;      // ���ٶȼ�����Ϊ:��16g         ��ȡ���ļ��ٶȼ����� ����2048       ����ת��Ϊ������λ�����ݣ���λ��g(m/s^2)
        default: break;
    }
    return acc_data;
}

float imu_gyro_transition (int16 gyro_value)
{
    float gyro_data = 0;
    switch(IMU_GYR_SAMPLE)
    {
        case 0x60: gyro_data = (float)gyro_value / 131.2f;  break;  //   ����������Ϊ:��250 dps     ��ȡ�������������ݳ���131           ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x40: gyro_data = (float)gyro_value / 65.6f;   break;  //   ����������Ϊ:��500 dps     ��ȡ�������������ݳ���65.5          ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x20: gyro_data = (float)gyro_value / 32.8f;   break;  //   ����������Ϊ:��1000dps     ��ȡ�������������ݳ���32.8          ����ת��Ϊ������λ�����ݣ���λΪ����/s
        case 0x00: gyro_data = (float)gyro_value / 16.4f;   break;  //   ����������Ϊ:��2000dps     ��ȡ�������������ݳ���16.4          ����ת��Ϊ������λ�����ݣ���λΪ����/s
        default: break;
    }
    return gyro_data;
}





void imu_get_acc (void)
{
    uint8 dat[6];

    imu_read_registers(IMU_ACCEL_DATA_XH, dat, 6);
    int16 acc_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    int16 acc_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    int16 acc_z = (int16)(((uint16)dat[4] << 8 | dat[5]));

    imu.acc_x=acc_x;
    imu.acc_y=acc_y;
    imu.acc_z=acc_z;

}

void imu_get_gyro (void)
{
    uint8 dat[6];

    imu_read_registers(IMU_GYRO_DATA_XH, dat, 6);
    int16 gyro_x = (int16)(((uint16)dat[0] << 8 | dat[1]));
    int16 gyro_y = (int16)(((uint16)dat[2] << 8 | dat[3]));
    int16 gyro_z = (int16)(((uint16)dat[4] << 8 | dat[5]));

    imu.gyro_x=gyro_x;
    imu.gyro_y=gyro_y;
    imu.gyro_z=gyro_z;
}





