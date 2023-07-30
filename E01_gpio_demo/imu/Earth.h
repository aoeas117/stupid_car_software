/*
 * Earth.h
 *
 *  Created on: 2023��4��10��
 *      Author: fuzic
 */

#ifndef IMU_EARTH_H_
#define IMU_EARTH_H_

#include "My_header.h"



/* WGS84����ģ�Ͳ���
   NOTE:���ʹ����������ģ����Ҫ�޸�������� */
#define WGS84_WIE  7.2921151467E-5;       /* ������ת���ٶ�*/
#define WGS84_F    0.0033528106647474805; /* ���� */
#define WGS84_RA   6378137.0;    /* ������a */
#define WGS84_RB   6356752.3142451793;    /* �̰���b */
#define WGS84_GM0  398600441800000.0;    /* ������������ */
#define WGS84_E1   0.0066943799901413156; /* ��һƫ����ƽ�� */
#define WGS84_E2   0.0067394967422764341; /* �ڶ�ƫ����ƽ�� */



class earth
{

public:
    /*
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:����������С
    */
    static float get_local_gravity_from_pos(Vector3d& pos)
    {
        //double sin2 = msin(pos[0]) * msin(pos[0]);
//        return 9.7803267715 * (1 + 0.0052790414 * sin2 + 0.0000232718 * sin2 * sin2) +
//            pos[2] * (0.0000000043977311 * sin2 - 0.0000030876910891) + 0.0000000000007211 * pos[2] * pos[2];

        return 9.7921481601393356;
    }
    /*
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:����������nϵ�е�ͶӰ
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
    pram:lat(����γ��)
    return:����Ȧ�뾶��î��Ȧ�뾶
    */
    static void get_local_Radius_from_latitude(double lat,float v[2])
    {

        float temp, sqrttmp;
        temp = 1 - sin(lat) * sin(lat) * WGS84_E1;
        sqrttmp = sqrt(temp);


        v[0]=  6378137.0 * (1 - 0.0066943799901413156) / (sqrttmp * temp),v[1]=6378137.0 / sqrttmp;
    }

    /*
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:nϵ(��������ϵ)��eϵ(���ĵع�����ϵ)ת������
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
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:nϵ(��������ϵ)��eϵ(���ĵع�����ϵ)ת����Ԫ��
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
    pram:q_n2e(nϵ��eϵ��ת����Ԫ����height(�߳�)
    return:(����λ������)γ�ȡ����ȡ��߳�
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
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:(����任����)nϵ���λ��ת�����������λ��
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
    pram:pos(����λ������)γ�ȡ����ȡ��߳�
    return:(����任����)�����������λ��תnϵ���λ��
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
    pram:lat(����λ�õ�γ��)
    return:(���ٶ�ͶӰ)������ת���ٶ�ͶӰ��nϵ
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
    pram:rmn(����Ȧ�뾶��î��Ȧ�뾶)pos((����λ������)γ�ȡ����ȡ��߳�)vel(�����ٶ���nϵ��ͶӰ)
    return:(���ٶ�ͶӰ)nϵ�����eϵ��ת�����ٶ�ͶӰ��nϵ
    */
    static Vector3f get_wne_n(float rmn[2],  Vector3d& pos,  Vector3f& vel)
    {
        Vector3f wne_n;// nϵ�����eϵת�����ٶ�ͶӰ��nϵ
        wne_n[0] = vel[1] / (rmn[1] + pos[2]),
        wne_n[1] = -vel[0] / (rmn[0] + pos[2]),
        wne_n[2] = -vel[1] * mtan(pos[0]) / (rmn[1] + pos[2]);
        return wne_n;
    }

};







#endif /* IMU_EARTH_H_ */
