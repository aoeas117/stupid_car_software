/*
 * icm.h
 *
 *  Created on: 2023年3月16日
 *      Author: 乘衣归
 */

#ifndef CODE_ICM_H_
#define CODE_ICM_H_


#include "zf_common_headfile.h"



#define ICM_FILTER_TIMES 50


typedef struct
{

   struct
   {
           uint8 filter_enable :1;//滤波器使能
           uint8 filter_count;
           float gyro_x;
           float gyro_y;
           float gyro_z;
           float acc_x;
           float acc_y;
           float acc_z;

   }filter;


   uint8 (*init)(void);
   void (*get_acc)(void);
   void (*get_gyro)(void);


}ICM_Administrators;

extern ICM_Administrators icm;




void icm_init(void);
void icm_get_data(void);
void icm_filter(void);
void icm_filter_enable(void);




#endif /* CODE_ICM_H_ */
