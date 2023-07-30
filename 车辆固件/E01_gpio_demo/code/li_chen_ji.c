#include "li_cheng_ji.h"


#define R 0.044//轮子半径m
#define RE 6378137//地球半径m


LOCATION_LICHENG_S location_licheng;
float sum_K1;

float ABS(float a)
{
	float b = a;
	if (a < 0)
		b = -b;
	return b;
}

/*
* 函数作用：基于电机在一段时间内的转动次数，推测走过的路径长，并基于航向角的偏移，假设改该过程为圆周运动进行位置更新，同时带精度参考。
* @param n：电机旋转次数（建议小于2转，否则推算精度较低，可写分数)单位：转
* @param yaw：现在的航向角，单位：rad
*/
void li_chen_ji_j(float n, float yaw)
{
	float dyaw = yaw - location_licheng.pre_yaw;
	if (dyaw > PI)
		dyaw = dyaw - 2*PI;
	if (dyaw < -1 * PI)
		dyaw = dyaw + 2 * PI;//计算变化角
	float r, y, x;
	if (dyaw != 0)
	{
		r = 2 * PI * R * n / ABS(dyaw);//计算等效转弯半径，可为负数
		y = r - r * cos(ABS(dyaw));//计算相对于之前的位移
		x = r * sin(ABS(dyaw));
		if (dyaw < 0)
			y = -y;
	}
	else
	{
		x = 2 * PI * R * n;
		y = 0;
	}
	float L_N, L_E;
	float H = location_licheng.height; float temp;
	L_N = cos(location_licheng.pre_yaw) * x - sin(location_licheng.pre_yaw) * y;
	L_E = cos(location_licheng.pre_yaw) * y + sin(location_licheng.pre_yaw) * x;//计算东北向位移m


	location_licheng.Latitude += L_N / (RE + H);
	location_licheng.Longitude += L_E * cos(location_licheng.Latitude) / (RE + H);//计算经纬度rad
	if (ABS(dyaw) < PI / 2)
	{
		temp = ABS(dyaw) - PI / 2 - sqrt(2);
		sum_K1 += 1 / temp / temp;
	}
	else if (ABS(dyaw) > PI / 2)
	{
		temp = ABS(dyaw) - PI / 2 + sqrt(2);
		sum_K1 += 1-1 / temp / temp;//更新里程计置信度总和,有角度变化决定
	}
	location_licheng.count++;//计数
	location_licheng.pre_yaw = yaw;//更新过去航向角
}

/*
* 函数作用：求两经纬度之间的距离，输入量rad单位
*/
float get_two_points_distance_f(double latitude1, double longitude1, double latitude2, double longitude2)
{
	float distance;
	double a;
	double b;

	a = latitude1 - latitude2;
	b = longitude1 - longitude2;

	distance = 2 * sin(sqrt(pow(sin(a / 2), 2) + cos(latitude1) * cos(latitude2) * pow(sin(b / 2), 2)));   // google maps 里面实现的算法
	distance = distance * RE;

	return distance;
}

/*
*函数作用：基于推算位置，对GPS的误差进行滤波
* @param  Latitude: 纬度 rad
* @param  Longitude: 经度 rad
* @param  h: 高度 m
* @param  hdop: 水平精度因子 m
*/
void GPS_update(double Latitude, double Longitude, float h, float hdop)
{
	float errL; float K1,K2=0,K=0;
	errL = get_two_points_distance_f(Latitude, Longitude, location_licheng.Latitude, location_licheng.Longitude);
	if (errL <= 0.8 * hdop)
	{
		K2 = 0;
	}
	else if (errL >= 5 * hdop)
		K2 = 1;
	else if (errL < 5 * hdop && errL> hdop)
		K2 = errL / (5 * hdop) - 1/5;
	if(location_licheng.count==0)
	    location_licheng.count=1;
	K1 = sum_K1 / location_licheng.count;//求里程计置信度，由总和取平均
	location_licheng.count = 0;
	K = K1 + K2;
	if (K > 1)
		K = 1;
	location_licheng.Latitude = location_licheng.Latitude + K * (Latitude - location_licheng.Latitude);
	location_licheng.Longitude = location_licheng.Longitude + K * (Longitude - location_licheng.Longitude);
	location_licheng.height = h;
}
/*
*函数作用：初始化内部参数
*/
void licheng_init(double la, double lo, float h, float yaw)
{
	location_licheng.Latitude = la;
	location_licheng.Longitude = lo;
	location_licheng.height = h;
	location_licheng.pre_yaw = yaw;
	location_licheng.count = 0;
}


void licheng_init_c(void)
{
    location_licheng.add_flag=0;
    location_licheng.gps_update_flag=0;
    location_licheng.used_flag=0;


}




void add_calculate(void)
{
    if(location_licheng.used_flag)
    {

        if(location_licheng.add_flag==12)
        {
            li_chen_ji_j(0.180604,ANGLE_TO_RAD(att_data.yaw));
            location_licheng.add_flag=0;
            control.sconcer_task.distance_count+=0.0499298;
        }
        if(location_licheng.gps_update_flag)
        {

            GPS_update(ANGLE_TO_RAD(gps_data.latitude),ANGLE_TO_RAD(gps_data.longitude),gps_data.height,gps_data.hoop_x);
            location_licheng.gps_update_flag=0;
        }
    }
}



