#include "li_cheng_ji.h"


#define R 0.044//���Ӱ뾶m
#define RE 6378137//����뾶m


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
* �������ã����ڵ����һ��ʱ���ڵ�ת���������Ʋ��߹���·�����������ں���ǵ�ƫ�ƣ�����ĸù���ΪԲ���˶�����λ�ø��£�ͬʱ�����Ȳο���
* @param n�������ת����������С��2ת���������㾫�Ƚϵͣ���д����)��λ��ת
* @param yaw�����ڵĺ���ǣ���λ��rad
*/
void li_chen_ji_j(float n, float yaw)
{
	float dyaw = yaw - location_licheng.pre_yaw;
	if (dyaw > PI)
		dyaw = dyaw - 2*PI;
	if (dyaw < -1 * PI)
		dyaw = dyaw + 2 * PI;//����仯��
	float r, y, x;
	if (dyaw != 0)
	{
		r = 2 * PI * R * n / ABS(dyaw);//�����Чת��뾶����Ϊ����
		y = r - r * cos(ABS(dyaw));//���������֮ǰ��λ��
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
	L_E = cos(location_licheng.pre_yaw) * y + sin(location_licheng.pre_yaw) * x;//���㶫����λ��m


	location_licheng.Latitude += L_N / (RE + H);
	location_licheng.Longitude += L_E * cos(location_licheng.Latitude) / (RE + H);//���㾭γ��rad
	if (ABS(dyaw) < PI / 2)
	{
		temp = ABS(dyaw) - PI / 2 - sqrt(2);
		sum_K1 += 1 / temp / temp;
	}
	else if (ABS(dyaw) > PI / 2)
	{
		temp = ABS(dyaw) - PI / 2 + sqrt(2);
		sum_K1 += 1-1 / temp / temp;//������̼����Ŷ��ܺ�,�нǶȱ仯����
	}
	location_licheng.count++;//����
	location_licheng.pre_yaw = yaw;//���¹�ȥ�����
}

/*
* �������ã�������γ��֮��ľ��룬������rad��λ
*/
float get_two_points_distance_f(double latitude1, double longitude1, double latitude2, double longitude2)
{
	float distance;
	double a;
	double b;

	a = latitude1 - latitude2;
	b = longitude1 - longitude2;

	distance = 2 * sin(sqrt(pow(sin(a / 2), 2) + cos(latitude1) * cos(latitude2) * pow(sin(b / 2), 2)));   // google maps ����ʵ�ֵ��㷨
	distance = distance * RE;

	return distance;
}

/*
*�������ã���������λ�ã���GPS���������˲�
* @param  Latitude: γ�� rad
* @param  Longitude: ���� rad
* @param  h: �߶� m
* @param  hdop: ˮƽ�������� m
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
	K1 = sum_K1 / location_licheng.count;//����̼����Ŷȣ����ܺ�ȡƽ��
	location_licheng.count = 0;
	K = K1 + K2;
	if (K > 1)
		K = 1;
	location_licheng.Latitude = location_licheng.Latitude + K * (Latitude - location_licheng.Latitude);
	location_licheng.Longitude = location_licheng.Longitude + K * (Longitude - location_licheng.Longitude);
	location_licheng.height = h;
}
/*
*�������ã���ʼ���ڲ�����
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



