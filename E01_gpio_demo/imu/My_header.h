/*
 * My_header.h
 *
 *  Created on: 2023Äê4ÔÂ9ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_MY_HEADER_H_
#define IMU_MY_HEADER_H_
extern "C"
{
#include "My_math.h"
}

#include "math.h"
#include "Matrix.h"
#include "Vector.h"
#include "Quaterniond.h"
#include "Transformer.h"
#include "Earth.h"
#include "imu_types.h"
#include "Ekf.h"


#include "sta_update.h"

#define dt 0.02
#define corr_time 1

extern "C"
{
    #include "data_set.h"
}





#endif /* IMU_MY_HEADER_H_ */
