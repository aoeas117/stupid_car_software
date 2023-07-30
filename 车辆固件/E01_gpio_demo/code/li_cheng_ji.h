#ifndef LI_CHENG_JI_H_
#define LI_CHENG_JI_H_



#include <math.h>
#include "data_set.h"
#include "gps.h"


typedef struct {
	double Latitude;//rad
	double Longitude;//rad
	float height;//m
	float pre_yaw;//上一时刻航向rad
	int count; //运算次数计数
	unsigned char gps_update_flag :1;
	unsigned char add_flag ;
	unsigned char used_flag :1;


}LOCATION_LICHENG_S;

extern LOCATION_LICHENG_S location_licheng;

void li_chen_ji_j(float n, float yaw);
void licheng_init_c(void);
void licheng_init(double la, double lo, float h, float yaw);
void GPS_update(double Latitude, double Longitude, float h, float hdop);
void add_calculate(void);
#endif
