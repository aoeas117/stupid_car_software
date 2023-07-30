/*
 * Earth.h
 *
 *  Created on: 2023年4月10日
 *      Author: fuzic
 */

#ifndef IMU_EARTH_H_
#define IMU_EARTH_H_

#include "My_header.h"



/* WGS84椭球模型参数
   NOTE:如果使用其他椭球模型需要修改椭球参数 */
#define WGS84_WIE  7.2921151467E-5;       /* 地球自转角速度*/
#define WGS84_F    0.0033528106647474805; /* 扁率 */
#define WGS84_RA   6378137.0;    /* 长半轴a */
#define WGS84_RB   6356752.3142451793;    /* 短半轴b */
#define WGS84_GM0  398600441800000.0;    /* 地球引力常数 */
#define WGS84_E1   0.0066943799901413156; /* 第一偏心率平方 */
#define WGS84_E2   0.0067394967422764341; /* 第二偏心率平方 */



class earth
{

public:
    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:当地重力大小
    */
    static float get_local_gravity_from_pos(Vector3d& pos)
    {
        //double sin2 = msin(pos[0]) * msin(pos[0]);
//        return 9.7803267715 * (1 + 0.0052790414 * sin2 + 0.0000232718 * sin2 * sin2) +
//            pos[2] * (0.0000000043977311 * sin2 - 0.0000030876910891) + 0.0000000000007211 * pos[2] * pos[2];

        return 9.7921481601393356;
    }
    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:当地重力在n系中的投影
    */
    static Vector3f get_local_gravity_n_from_pos(Vector3d& pos)
    {
        Vector3f g_n;
        float g_local = get_local_gravity_from_pos(pos);
        g_n[0] = 0;
        g_n[1] = 0;
        g_n[2] = g_local;

        return g_n;
    }
    /*
    pram:lat(载体纬度)
    return:子午圈半径和卯酉圈半径
    */
    static void get_local_Radius_from_latitude(double lat,float v[2])
    {

        float temp, sqrttmp;
        temp = 1 - sin(lat) * sin(lat) * WGS84_E1;
        sqrttmp = sqrt(temp);


        v[0]=  6378137.0 * (1 - 0.0066943799901413156) / (sqrttmp * temp),v[1]=6378137.0 / sqrttmp;
    }

    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:n系(导航坐标系)到e系(地心地固坐标系)转换矩阵
    */
    static MatrixXf get_matrix_n2e_from_pos(Vector3d& pos)
    {
        float coslon, sinlon, coslat, sinlat;

        sinlat = sin(pos[0]);
        sinlon = sin(pos[1]);
        coslat = cos(pos[0]);
        coslon = cos(pos[1]);

        MatrixXf m_n2e(3,3);
        m_n2e[0][0] = -sinlat * coslon;
        m_n2e[0][1] = -sinlon;
        m_n2e[0][2] = -coslat * coslon;

        m_n2e[1][0] = -sinlat * sinlon;
        m_n2e[1][1] = coslon;
        m_n2e[1][2] = -coslat * sinlon;

        m_n2e[2][0] = coslat;
        m_n2e[2][1] = 0;
        m_n2e[2][2] = -sinlat;

        return m_n2e;
    }

    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:n系(导航坐标系)到e系(地心地固坐标系)转换四元数
    */
    static Quaterniond get_quaterniond_n2e_from_pos(Vector3d& pos)
    {
        Quaterniond q_n2e;

        float coslon, sinlon, coslat, sinlat;

        coslon = cos(pos[1] * 0.5);
        sinlon = sin(pos[1] * 0.5);
        coslat = cos(-3.14159265358979323846 /4.0 - pos[0] * 0.5);
        sinlat = sin(-3.14159265358979323846 /4.0 - pos[0] * 0.5);

        q_n2e.w = coslat * coslon;
        q_n2e.x = -sinlat * sinlon;
        q_n2e.y = sinlat * coslon;
        q_n2e.z = coslat * sinlon;

        return q_n2e;
    }

    /*
    pram:q_n2e(n系到e系的转换四元数）height(高程)
    return:(载体位置向量)纬度、经度、高程
    */
    static Vector3d get_position_from_qn2e(const Quaterniond& q_n2e, double height)
    {
        Vector3d pos;
        pos[0] = -2 * matan(q_n2e.y / q_n2e.w) - 3.14159265358979323846 / 2.0;
        pos[1] = 2 * matan2(q_n2e.z, q_n2e.w);
        pos[2]= height;
        return pos;
    }


    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:(坐标变换矩阵)n系相对位置转地理坐标相对位置
    */
    static MatrixXf get_matrix_n2geo_pos(Vector3d& pos)
    {
        MatrixXf dri(3,3);

        float rmn[2];
        get_local_Radius_from_latitude(pos[0],rmn);

        dri[0][0] = 1.0 / (rmn[0] + pos[2]);
        dri[1][1] = 1.0 / ((rmn[1] + pos[2]) * cos(pos[0]));
        dri[2] [2] = -1;
        return dri;
    }

    /*
    pram:pos(载体位置向量)纬度、经度、高程
    return:(坐标变换矩阵)地理坐标相对位置转n系相对位置
    */
    static MatrixXf get_matrix_geo2n_pos(Vector3d& pos)
    {
        MatrixXf dr(3,3);

        float rmn[2];
        get_local_Radius_from_latitude(pos[0],rmn);

        dr[0][0] = rmn[0] + pos[2];
        dr[1][1] = (rmn[1] + pos[2]) * cos(pos[0]);
        dr[2][2] = -1;
        return dr;
    }


    /*
    pram:lat(载体位置的纬度)
    return:(角速度投影)地球自转角速度投影到n系
    */
    static Vector3f get_wei_n(double lat)
    {
        Vector3f wei_n;
        wei_n[0] = 7.2921151467E-5 * cos(lat),
        wei_n[1] = 0,
        wei_n[2] = -7.2921151467E-5 * sin(lat);
        return wei_n;
    }


    /*
    pram:rmn(子午圈半径和卯酉圈半径)pos((载体位置向量)纬度、经度、高程)vel(载体速度在n系的投影)
    return:(角速度投影)n系相对于e系的转动角速度投影到n系
    */
    static Vector3f get_wne_n(float rmn[2],  Vector3d& pos,  Vector3f& vel)
    {
        Vector3f wne_n;// n系相对于e系转动角速度投影到n系
        wne_n[0] = vel[1] / (rmn[1] + pos[2]),
        wne_n[1] = -vel[0] / (rmn[0] + pos[2]),
        wne_n[2] = -vel[1] * mtan(pos[0]) / (rmn[1] + pos[2]);
        return wne_n;
    }

};







#endif /* IMU_EARTH_H_ */
