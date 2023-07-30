/*
 * gps.h
 *
 *  Created on: 2023��3��17��
 *      Author: ���¹�
 */

#ifndef CODE_GPS_H_
#define CODE_GPS_H_
#include "zf_common_headfile.h"


#define GPS_GET_DATA_TIMES 5//�ȵ��̬�ֶ�׼��ʱ��ȡ���ٴ�����(�˲���)

#define GPS_POINT_TOTAL 32 //ȫ��·���ܲȵ���





typedef union
{
        float double_type;
        unsigned long flash_data[2];
}change_union;






typedef struct
{
        float *latitude_point;
        float *longitude_point;
        float  *height_point;

        float init_yaw;
        struct
        {
          unsigned char filter_enable :1;      //1:ʹ�� 0:ʧ��  �˲���ʹ�ܱ�־λ
          unsigned char fiter_work_mode :1;    //1�ȵ��˲� 0����׼�˲�
          unsigned char count_rmc :7;
          unsigned char count_gngga :7;
          float *filter_latitude_point;
          float *filter_longitude_point;
          float *filter_height_point;
          float *filter_hdop;
        }filter;

        struct
        {
                unsigned char index;//��������
                float latitude;
                float height;
                float longitude;

        }gps_data;//׼ȷ����

        unsigned char gps_data_used_flag_rmc :1;//1:δʹ�� 0:ʹ����
        unsigned char gps_data_used_flag_gngga :1;
        unsigned char gps_data_used_flag_gngsa :1;

}GPS_Administrator;


extern GPS_Administrator gps;


uint8 gps_get_end_point     (GPS_Administrator *GA);
void gps_Administrator_init (GPS_Administrator *GA);
void gps_filter             (GPS_Administrator *GA);
void gps_save_point         (GPS_Administrator *GA,unsigned char index);
void gps_static_target      (GPS_Administrator *GA);
void gps_read_point         (GPS_Administrator *GA,unsigned char index);
void gps_delete_point       (GPS_Administrator *GA,unsigned char index);
void gps_write_flash        (GPS_Administrator *GA);//���ȵ�����д��flash
void gps_read_flash         (GPS_Administrator *GA);//��flash�ж�ȡ�ȵ�����
uint8 gps_judge_null        (GPS_Administrator *GA,uint8 index);
uint8 gps_count_not_null    (GPS_Administrator *GA);//ͳ�Ʒǿյ���
void gps_send_saved_point   (GPS_Administrator *GA);
void gps_get_data           (void);

void save_static_data(void);//ÿҳ113��
void read_static_data(void);
void read_direct_data(void);


void gps_average_filter     (float *lat,float *lon,uint8 count,GPS_Administrator *GA);//ƽ���˲�
void gps_overlap_filter     (float *lat,float *lon,float *hdop,uint8 *over_count,uint8 count,GPS_Administrator *GA);//�ص��˲�




#endif /* CODE_GPS_H_ */
