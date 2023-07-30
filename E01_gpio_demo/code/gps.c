/*
 * gps.c
 *
 *  Created on: 2023年3月17日
 *      Author: 乘衣归
 */

#include "gps.h"

float gps_height_buffer[GPS_POINT_TOTAL];
float gps_latitude_buffer[GPS_POINT_TOTAL];//踩点数组latitude
float gps_longitude_buffer[GPS_POINT_TOTAL];//踩点数组longitude

float data_filter_buffer1[GPS_GET_DATA_TIMES];//latitude
float data_filter_buffer2[GPS_GET_DATA_TIMES];//longitude
float data_filter_buffer3[GPS_GET_DATA_TIMES];//height
float data_filter_buffer4[GPS_GET_DATA_TIMES];//水平精度
uint8 overlap_count[GPS_GET_DATA_TIMES];//重叠次数

GPS_Administrator gps;



void gps_get_data(void)
{

    if(gps.gps_data_used_flag_gngga&&gps.gps_data_used_flag_gngsa&&gps.gps_data_used_flag_rmc)
       {
           gps.gps_data_used_flag_gngga=0;
           gps.gps_data_used_flag_gngsa=0;
           gps.gps_data_used_flag_rmc=0;

           gps_data.latitude=gps_tau1201.latitude;
           gps_data.longitude=gps_tau1201.longitude;
           gps_data.height=gps_tau1201.height;

           gps_data.yaw=gps_tau1201.direction;//rad
           gps_data.speed=gps_tau1201.speed/(3.6);//m/s


           gps_data.hoop_x=gps_tau1201.hdop;
           gps_data.hoop_y=gps_tau1201.hdop;
           gps_data.hoop_z=gps_tau1201.vdop;

           location_licheng.gps_update_flag=1;

           if(gps_data.speed>5 || motor->motor_speed.current_speed>1300)
           {

               gps.init_yaw=gps_data.yaw;

           }

           if(location_licheng.used_flag==0 && icm.filter.filter_enable==0)//里程计未开启且姿态解算开始
           {
               licheng_init(ANGLE_TO_RAD(gps_data.latitude),ANGLE_TO_RAD(gps_data.longitude),gps_data.height,ANGLE_TO_RAD(att_data.yaw));
               location_licheng.used_flag=1;

           }


           gps_data.update_flag=1;
       }





}



void gps_Administrator_init(GPS_Administrator *GA)
{
    GA->filter.count_rmc=0;
    GA->filter.count_gngga=0;
    GA->filter.filter_enable=0;
    GA->filter.fiter_work_mode=1;
    GA->filter.filter_latitude_point=data_filter_buffer1;
    GA->filter.filter_longitude_point=data_filter_buffer2;
    GA->filter.filter_height_point=data_filter_buffer3;
    GA->filter.filter_hdop=data_filter_buffer4;

    GA->gps_data.index=255;
    GA->gps_data.latitude=0.0;
    GA->gps_data.longitude=0.0;
    GA->gps_data.height=0.0;

    GA->gps_data_used_flag_rmc=0;
    GA->gps_data_used_flag_gngga=0;
    GA->gps_data_used_flag_gngsa=0;

    GA->latitude_point=gps_latitude_buffer;
    GA->longitude_point=gps_longitude_buffer;
    GA->height_point=gps_height_buffer;
};






void gps_filter(GPS_Administrator *GA)
{
    if(GA->filter.filter_enable==1)//如果滤波器使能
    {
        if(GA->gps_data_used_flag_rmc==1&&GA->filter.count_rmc!=GPS_GET_DATA_TIMES)
        {
            GA->filter.filter_latitude_point[GA->filter.count_rmc]=gps_tau1201.latitude;
            GA->filter.filter_longitude_point[GA->filter.count_rmc]=gps_tau1201.longitude;
            GA->filter.filter_hdop[GA->filter.count_rmc]=gps_tau1201.hdop;


            if(gps_tau1201.latitude!=0&&gps_tau1201.longitude!=0)
                GA->filter.count_rmc++;

            GA->gps_data_used_flag_rmc=0;
        }

        if(GA->gps_data_used_flag_gngga==1&&GA->filter.count_gngga!=GPS_GET_DATA_TIMES)
        {
            GA->filter.filter_height_point[GA->filter.count_gngga++]=gps_tau1201.height;

            GA->gps_data_used_flag_gngga=0;
        }


        if(GA->filter.count_rmc==GPS_GET_DATA_TIMES)//滤波数据已收集（开始滤波）
        {
           //gps_average_filter(GA->filter.filter_latitude_point,GA->filter.filter_longitude_point,GPS_GET_DATA_TIMES,GA);
           //gps_overlap_filter(GA->filter.filter_latitude_point,GA->filter.filter_longitude_point,GA->filter.filter_hdop,overlap_count,GPS_GET_DATA_TIMES,GA);


           if(GA->filter.fiter_work_mode==1)//踩点滤波
           {
               GA->latitude_point[GA->gps_data.index]=gps_tau1201.latitude;
               GA->longitude_point[GA->gps_data.index]=gps_tau1201.longitude;
           }else
           {
               gps_data.latitude=(PI/180.0)*(GA->gps_data.latitude);//转换为弧度制
               gps_data.longitude=(PI/180.0)*(GA->gps_data.longitude);
           }


        }


        if(GA->filter.count_gngga==GPS_GET_DATA_TIMES)
        {
            float max=GA->filter.filter_height_point[0],min=GA->filter.filter_height_point[0],sum=0.0;

            for(uint8 i=0;i<GPS_GET_DATA_TIMES;i++)
            {
                if(GA->filter.filter_height_point[i]>max)
                    max=GA->filter.filter_height_point[i];
                if(GA->filter.filter_height_point[i]<min)
                    min=GA->filter.filter_height_point[i];
                sum+=GA->filter.filter_height_point[i];
            }
            GA->gps_data.height=(sum-max-min)/(GPS_GET_DATA_TIMES-2);

            if(GA->filter.fiter_work_mode==1)//踩点滤波
            {
                GA->height_point[GA->gps_data.index]=GA->gps_data.height;
            }else
            {
                gps_data.height=GA->gps_data.height;
            }
        }
        if(GA->filter.count_rmc==GPS_GET_DATA_TIMES && GA->filter.count_gngga==GPS_GET_DATA_TIMES)
        {
            if(GA->filter.fiter_work_mode==0)
                gps_data.update_flag=1;
            else
            {
                gps_write_flash(GA);
                char string[6];
                sprintf(string,"%u",GA->gps_data.index+1);
                lora_send_str(&lora,string);
            }
            GA->filter.filter_enable=0;//关闭滤波器
        }
    }
}



void gps_save_point(GPS_Administrator *GA,uint8 index)//将踩点存入数组
{
    if(index>GPS_POINT_TOTAL || index==0)
    {
        lora_send_str(&lora,"exit error");
        return;
    }
    GA->filter.count_gngga=0;
    GA->filter.count_rmc=0;
    GA->filter.filter_enable=1;//开启滤波器
    GA->filter.fiter_work_mode=1;//滤波器工作在踩点模式
    GA->gps_data.index=index-1;//选择踩点索引

    if(gps_judge_null(GA,index))
        lora_send_str(&lora,"gpss");
    else
       lora_send_str(&lora,"changed!");



}


void gps_static_target(GPS_Administrator *GA)
{
    GA->filter.count_gngga=0;
    GA->filter.count_rmc=0;
    GA->filter.filter_enable=1;//开启滤波器
    GA->filter.fiter_work_mode=0;//滤波器工作在对准模式



}








void gps_read_point(GPS_Administrator *GA,uint8 index)//从数组中读取踩点
{



    if(index>GPS_POINT_TOTAL)
    {
        lora_send_str(&lora,"exit error");
        return;
    }

    index=index-1;


    if(index==255)
    {
        gps.gps_data.latitude=gps_tau1201.latitude;
        gps.gps_data.longitude=gps_tau1201.longitude;
        gps.gps_data.height=gps_tau1201.height;


    }else
    {
        gps.gps_data.latitude=gps.latitude_point[index];
        gps.gps_data.longitude=gps.longitude_point[index];
        gps.gps_data.height=gps.height_point[index];

    }


    lora_encode_send_data(&lora,latitude);

    system_delay_ms(200);

    lora_encode_send_data(&lora,longitude);

    system_delay_ms(200);

    lora_encode_send_data(&lora,height);

    system_delay_ms(200);


}


void gps_delete_point(GPS_Administrator *GA,uint8 index)//从数组中删除踩点
{
    if(index>GPS_POINT_TOTAL || index==0)
   {
       lora_send_str(&lora,"exit error");
       return;
   }

    if(gps_judge_null(GA,index-1))
    {
        lora_send_str(&lora,"no data");
        return;
    }

    GA->gps_data.index=index-1;//选择索引
    GA->latitude_point[GA->gps_data.index]=0.0;
    GA->longitude_point[GA->gps_data.index]=0.0;
    GA->height_point[GA->gps_data.index]=0.0;

    gps_write_flash(GA);

    lora_send_str(&lora,"gpsd");//已删除响应
}



//
void gps_write_flash(GPS_Administrator *GA)
{
    flash_buffer_clear();//清缓存区
    if(flash_check(0,11))//如果有数据，其他参数
    {
        flash_read_page_to_buffer(0,11);// 向缓冲区写入指定 FLASH 的扇区的指定页码的数据
    }

    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        flash_union_buffer[i].float_type=GA->latitude_point[i];
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        flash_union_buffer[i+GPS_POINT_TOTAL].float_type=GA->longitude_point[i];
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        flash_union_buffer[(GPS_POINT_TOTAL)*2+i].float_type=GA->height_point[i];

    if(flash_check(0,11))
   {
       flash_erase_page(0,11);//先擦除片区
       flash_write_page_from_buffer(0,11);// 向指定 FLASH 的扇区的指定页码写入缓冲区的数据
   }else
   {
       flash_write_page_from_buffer(0,11);
   }

}




//
//void gps_write_flash(GPS_Administrator *GA)//将踩点数据写入flash
//{
//    flash_buffer_clear();//清缓存区
//    change_union temp_u;
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//        temp_u.double_type=GA->latitude_point[i];
//        flash_union_buffer[i*2].uint32_type=temp_u.flash_data[0];
//        flash_union_buffer[i*2+1].uint32_type=temp_u.flash_data[1];
//    }
//
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//        temp_u.double_type=GA->longitude_point[i];
//        flash_union_buffer[(GPS_POINT_TOTAL+i)*2].uint32_type=temp_u.flash_data[0];
//        flash_union_buffer[(GPS_POINT_TOTAL+i)*2+1].uint32_type=temp_u.flash_data[1];
//    }
//
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//        flash_union_buffer[(GPS_POINT_TOTAL)*4+i].float_type=GA->height_point[i];
//    }
//
//    if(flash_check(0,11))
//    {
//        flash_erase_page(0,11);//先擦除片区
//        flash_write_page_from_buffer(0,11);// 向指定 FLASH 的扇区的指定页码写入缓冲区的数据
//    }else
//    {
//        flash_write_page_from_buffer(0,11);
//    }
//}


void gps_read_flash(GPS_Administrator *GA)
{
    flash_buffer_clear();//清缓存区
    if(flash_check(0,11))
    {
        flash_read_page_to_buffer(0,11);
    }else
    {
        lora_send_str(&lora,"no data");
        return;
    }

    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        GA->latitude_point[i]=flash_union_buffer[i].float_type;
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        GA->longitude_point[i]=flash_union_buffer[i+GPS_POINT_TOTAL].float_type;
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        GA->height_point[i]=flash_union_buffer[(GPS_POINT_TOTAL)*2+i].float_type;

}




//void gps_read_flash(GPS_Administrator *GA)//从flash中读取踩点数据
//{
//    change_union temp_u;
//    flash_buffer_clear();//清缓存区
//    if(flash_check(0,11))
//    {
//        flash_read_page_to_buffer(0,11);
//    }else
//    {
//        lora_send_str(&lora,"no data");
//        return;
//    }
//
//
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//        temp_u.flash_data[0]=flash_union_buffer[i*2].uint32_type;
//        temp_u.flash_data[1]=flash_union_buffer[i*2+1].uint32_type;
//        GA->latitude_point[i]=temp_u.double_type;
//    }
//
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//       temp_u.flash_data[0]=flash_union_buffer[(GPS_POINT_TOTAL+i)*2].uint32_type;
//       temp_u.flash_data[1]=flash_union_buffer[(GPS_POINT_TOTAL+i)*2+1].uint32_type;
//       GA->longitude_point[i]=temp_u.double_type;
//    }
//
//
//
//    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
//    {
//        GA->height_point[i]=flash_union_buffer[(GPS_POINT_TOTAL)*4+i].float_type;
//    }
//}




uint8 gps_judge_null(GPS_Administrator *GA,uint8 index)//1:踩点为空
{
    if(index>GPS_POINT_TOTAL) return 255;//error

    if(GA->latitude_point[index]==0.0 || GA->longitude_point[index]==0)
        return 1;
    return 0;
}



uint8 gps_count_not_null(GPS_Administrator *GA)//统计非空点数
{
    uint8 count=0;
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        if(gps_judge_null(GA,i)==0)
            count++;
    return count;
}


uint8 gps_get_end_point(GPS_Administrator *GA)
{
    for(uint8 i=0;i<GPS_POINT_TOTAL;i++)
        if(gps_judge_null(GA,i))
            return i;
    return 32;
}



void gps_send_saved_point(GPS_Administrator *GA)
{
    uint8 sum=gps_count_not_null(GA);

    char string[10];

    sprintf(string,"gps%u/%u",sum,GPS_POINT_TOTAL);
    lora_send_str(&lora,string);

}


void save_static_data(void)//每页113个
{
    static unsigned char complete_flag=1;

    static unsigned char data_count=0;
    static unsigned char page_index=0;
    if(complete_flag==1)
    {

        flash_union_buffer[data_count*6+0].float_type=location_licheng.Latitude;
        flash_union_buffer[data_count*6+1].float_type=location_licheng.Longitude;
        flash_union_buffer[data_count*6+2].float_type=location_licheng.height;
        flash_union_buffer[data_count*6+3].float_type=gps_data.latitude;
        flash_union_buffer[data_count*6+4].float_type=gps_data.longitude;
        flash_union_buffer[data_count*6+5].float_type=gps_data.height;
        data_count++;
        if(data_count==170)
        {
            if(flash_check(0,page_index))
           {
               flash_erase_page(0,page_index);//先擦除片区
               flash_write_page_from_buffer(0,page_index);// 向指定 FLASH 的扇区的指定页码写入缓冲区的数据
           }else
           {
               flash_write_page_from_buffer(0,page_index);
           }
            flash_buffer_clear();//清缓存区
            page_index++;

            if(page_index==11)
            {
                complete_flag=0;
                lora_send_str(&lora,"complete");
            }

        }
    }

}


void read_static_data(void)
{

    static unsigned char read_complete_flag=1;
    static unsigned char read_index=0;
    static unsigned char read_data_count=0;
    if(read_complete_flag)
    {
        if(read_data_count==0)
            if(flash_check(0,read_index))
            {
                flash_buffer_clear();//清缓存区
                flash_read_page_to_buffer(0,read_index);
            }
        printf("%f,%f,%f,%f,%f,%f\n"\
                ,flash_union_buffer[read_data_count*6+0].float_type,flash_union_buffer[read_data_count*6+1].float_type\
                ,flash_union_buffer[read_data_count*6+2].float_type,flash_union_buffer[read_data_count*6+3].float_type\
                ,flash_union_buffer[read_data_count*6+4].float_type,flash_union_buffer[read_data_count*6+5].float_type);

        read_data_count++;
        if(read_data_count==170)
        {
            read_index++;
            read_data_count=0;
            if(read_index==11)
                read_complete_flag=0;
        }
    }

}



void read_direct_data(void)
{
    printf("%f %f %f %f %f %f %f %f %f\n"\
            ,vel_data.vel_x,vel_data.vel_y,vel_data.vel_z\
            ,pos_data.latitude,pos_data.longitude,pos_data.height\
            ,att_data.pitch,att_data.roll,att_data.yaw);

    //printf("%f %f %f %f %f %f \n",imu_bias.gyro_x,imu_bias.gyro_y,imu_bias.gyro_z,imu_bias.acc_x,imu_bias.acc_y,imu_bias.acc_z);
   // printf("%f %f %f %f %f %f \n",imu_data.gyro_x,imu_data.gyro_y,imu_data.gyro_z,imu_data.acc_x,imu_data.acc_y,imu_data.acc_z);

}


void gps_average_filter(float *lat,float *lon,uint8 count,GPS_Administrator *GA)//平均滤波
{
    float max=lat[0],min=lat[0],sum=0.0;
    for(uint8 i=0;i<count;i++)
    {
        if(lat[i]>max)
            max=lat[i];
        if(lat[i]<min)
            min=lat[i];
        sum+=lat[i];
    }
    GA->gps_data.latitude=(sum-max-min)/(count-2);

    max=lon[0],min=lon[0],sum=0.0;
    for(uint8 i=0;i<count;i++)
   {
       if(lon[i]>max)
           max=lon[i];
       if(lon[i]<min)
           min=lon[i];
       sum+=lon[i];
   }
   GA->gps_data.longitude=(sum-max-min)/(count-2);



}


void gps_overlap_filter(float *lat,float *lon,float *hdop,uint8 *over_count,uint8 count,GPS_Administrator *GA)//重叠滤波
{
    double distance=0;
    for(uint8 i=0;i<count;i++)
    {
        over_count[i]=0;
        for(uint8 j=0;j<count;j++)
        {
            if(i!=j)
            {
                distance=get_two_points_distance(lat[i],lon[i],lat[j],lon[j]);
                if(distance<hdop[i]+hdop[j])
                    over_count[i]++;
            }
        }
    }
    uint8 max_overlap=over_count[0];
    for(uint8 i=1;i<count;i++)
        if(over_count[i]>max_overlap)
            max_overlap=over_count[i];
    float sum_lat=0,sum_lon=0;
    uint8 sum_count=0;
    for(uint8 i=0;i<count;i++)
        if(over_count[i]==max_overlap)
        {
            sum_lat+=lat[i];
            sum_lon+=lon[i];
            sum_count++;
        }

    //计算经纬度
    GA->gps_data.latitude=sum_lat/sum_count;
    GA->gps_data.longitude=sum_lon/sum_count;

}













