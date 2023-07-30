/*
 * gps.h
 *
 *  Created on: 2023年3月17日
 *      Author: 乘衣归
 */

#ifndef CODE_GPS_H_
#define CODE_GPS_H_
#include "zf_common_headfile.h"


#define GPS_GET_DATA_TIMES 5//踩点或静态粗对准的时候取多少次数据(滤波用)

#define GPS_POINT_TOTAL 32 //全段路程总踩点数





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
          unsigned char filter_enable :1;      //1:使能 0:失能  滤波器使能标志位
          unsigned char fiter_work_mode :1;    //1踩点滤波 0：对准滤波
          unsigned char count_rmc :7;
          unsigned char count_gngga :7;
          float *filter_latitude_point;
          float *filter_longitude_point;
          float *filter_height_point;
          float *filter_hdop;
        }filter;

        struct
        {
                unsigned char index;//点序索引
                float latitude;
                float height;
                float longitude;

        }gps_data;//准确数据

        unsigned char gps_data_used_flag_rmc :1;//1:未使用 0:使用了
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
void gps_write_flash        (GPS_Administrator *GA);//将踩点数据写入flash
void gps_read_flash         (GPS_Administrator *GA);//从flash中读取踩点数据
uint8 gps_judge_null        (GPS_Administrator *GA,uint8 index);
uint8 gps_count_not_null    (GPS_Administrator *GA);//统计非空点数
void gps_send_saved_point   (GPS_Administrator *GA);
void gps_get_data           (void);

void save_static_data(void);//每页113个
void read_static_data(void);
void read_direct_data(void);


void gps_average_filter     (float *lat,float *lon,uint8 count,GPS_Administrator *GA);//平均滤波
void gps_overlap_filter     (float *lat,float *lon,float *hdop,uint8 *over_count,uint8 count,GPS_Administrator *GA);//重叠滤波




#endif /* CODE_GPS_H_ */
