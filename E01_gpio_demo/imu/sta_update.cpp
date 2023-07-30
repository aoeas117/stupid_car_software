/*
 * sta_update.cpp
 *
 *  Created on: 2023年4月10日
 *      Author: fuzic
 */
#include "sta_update.h"



SOLUTION update;


void my_update::carrier_sta_init(IMU_DATA &imu_data,GPS_DATA &gps_data,EKF_STA &pre,EKF_STA &cur)//载体初始对准
{
    //位置
    cur.pos[0] = gps_data.latitude,
    cur.pos[1] = gps_data.longitude,
    cur.pos[2] = gps_data.height;

    Vector3f g_n, g_b;
    g_n = earth::get_local_gravity_n_from_pos(cur.pos);
    g_b[0] = -imu_data.acc_x, g_b[1] = -imu_data.acc_y, g_b[2] = -imu_data.acc_z;



    Vector3f wei_n, wei_b;
    wei_n = earth::get_wei_n(cur.pos[0]);
    wei_b[0] = imu_data.gyro_x, wei_b[1] = imu_data.gyro_y, wei_b[2] = imu_data.gyro_z;

    Vector3f vg, vw, vgw;
    vg = g_n.get_normalize();
    vw = g_n.cross(wei_n).get_normalize();
    vgw = g_n.cross(wei_n).cross(g_n).get_normalize();

    Vector3f wg, ww, wgw;
    wg = g_b.get_normalize();
    ww = g_b.cross(wei_b).get_normalize();
    wgw = g_b.cross(wei_b).cross(g_b).get_normalize();

    MatrixXf m1(3, 3), m2(3, 3); MatrixXf m_t(3, 1);
    m_t = vg.vector_column(); m1.set_Block_Matrix(m_t, 0, 0);
    m_t = vw.vector_column(); m1.set_Block_Matrix(m_t, 0, 1);
    m_t = vgw.vector_column(); m1.set_Block_Matrix(m_t, 0, 2);

    m_t.Transpose();
    m_t = wg.vector_row(); m2.set_Block_Matrix(m_t, 0, 0);
    m_t = ww.vector_row(); m2.set_Block_Matrix(m_t, 1, 0);
    m_t = wgw.vector_row(); m2.set_Block_Matrix(m_t, 2, 0);

    cur.att.a_matrix = m1 * m2;
    cur.att.a_euler = Rotation::matrix2euler(cur.att.a_matrix);
    cur.att.a_quaterniond = Rotation::euler2quaternion(cur.att.a_euler);

    cur.vel[0] = 0, cur.vel[1] = 0, cur.vel[1] = 0;

    pre = cur;

}



void my_update::vel_update( EKF_STA &pre,EKF_STA &cur)
{

    float local_r[2];
    earth::get_local_Radius_from_latitude(pre.pos[0], local_r);

    Vector3f local_gn;
    local_gn = earth::get_local_gravity_n_from_pos(pre.pos);

    Vector3f wei_n, wne_n;
    wei_n = earth::get_wei_n(pre.pos[0]), wne_n = earth::get_wne_n(local_r, pre.pos, pre.vel);

    Vector3f t1, t2, t3;
    t1 = cur.imu.dtheta.cross(cur.imu.dvel) * (1.0 / 2),//corss函数表示叉乘向量，前者叉乘后者
    t2 = pre.imu.dtheta.cross(cur.imu.dvel) * (1.0 / 12),
    t3 = pre.imu.dvel.cross(cur.imu.dtheta) * (1.0 / 12);

    Vector3f d_vfb;
    d_vfb = cur.imu.dvel + t1 + t2 + t3;


    MatrixXf I33(3, 3);
    I33.set_Identity();
    t1 = (wei_n + wne_n) * (dt / 2);//需要用中间时刻的数据
    MatrixXf cnn(3, 3);
    cnn = I33 - t1.get_Antisymmetric_Matrix();//3*3的单位阵-由三维向量temp1构造的反对称矩阵，误差

    MatrixXf vector_t(3, 1);
    Vector3f d_vfn;
    vector_t = cnn * pre.att.a_matrix * d_vfb.vector_column();
    d_vfn = Rotation::matrix2vector3d(vector_t);


    Vector3f d_vgn; d_vgn = (local_gn - (wei_n * 2 + wne_n).cross(pre.vel)) * dt;

    Vector3f midvel;Vector3d  midpos;
    midvel = pre.vel + (d_vfn + d_vgn) * 0.5;


    Quaterniond qnn, qee, qne;
    qnn = Rotation::rotvec2quaternion(t1);
    t2[0] = 0, t2[1] = 0, t2[2] = -7.2921151467E-5 * dt / 2;
    qee = Rotation::rotvec2quaternion(t2);//地球坐标系从t-1到（2t-1）/2时间内对于惯性坐标系的姿态变化四元数
    qne = earth::get_quaterniond_n2e_from_pos(pre.pos);//导航坐标系的三个轴可以通过位置确定
    qne = qee * qne * qnn;
    midpos[2] = pre.pos[2] - midvel[2] * (dt / 2);
    midpos = earth::get_position_from_qn2e(qne, midpos[2]);

    earth::get_local_Radius_from_latitude(midpos[0], local_r);
    wei_n = earth::get_wei_n(midpos[0]);
    wne_n = earth::get_wne_n(local_r, midpos, midvel);

    local_gn = earth::get_local_gravity_n_from_pos(midpos);

    t1 = (wei_n + wne_n) * (dt / 2);
    cnn = I33 - t1.get_Antisymmetric_Matrix();
    vector_t = cnn * pre.att.a_matrix * d_vfb.vector_column();
    d_vfn = Rotation::matrix2vector3d(vector_t);

    d_vgn = (local_gn - (wei_n * 2 + wne_n).cross(midvel)) * dt;

    cur.vel = pre.vel + d_vfn + d_vgn;
}


void my_update::pos_update( EKF_STA &pre,EKF_STA &cur)
{
    MatrixXf vector_t(3, 1);
    Vector3f   midvel, midpos;
    midvel = (cur.vel + pre.vel) * 0.5;
    vector_t = earth::get_matrix_n2geo_pos(pre.pos) *midvel.vector_column() * dt;
    Vector3f temp_v3f;
    temp_v3f=Rotation::matrix2vector3d(vector_t);
    cur.pos[0] = pre.pos[0] + temp_v3f[0]; cur.pos[1] = pre.pos[1] + temp_v3f[1]; cur.pos[2] = pre.pos[2] + temp_v3f[2];
//
//    float  local_r[2];
//    earth::get_local_Radius_from_latitude(midpos[0], local_r);
//    Vector3f  wei_n, wne_n;
//    wei_n = earth::get_wei_n(midpos[0]),
//    wne_n = earth::get_wne_n(local_r, midpos, midvel);
//
//    Vector3f t1;
//    t1 = (wei_n + wne_n) * dt;
//    Quaterniond qnn;
//    qnn = Rotation::rotvec2quaternion(t1);
//
//    t1[0] = 0, t1[1] = 0, t1[2] = -7.2921151467E-5 * dt;
//    Quaterniond qee;
//    qee = Rotation::rotvec2quaternion(t1);
//
//    Quaterniond qne;
//    qne = earth::get_quaterniond_n2e_from_pos(pre.pos);
//    qne = qee * qne * qnn;
//    cur.pos[2] = pre.pos[2] - midvel[2] * dt;
//    cur.pos = earth::get_position_from_qn2e(qne, cur.pos[2]);
}


void my_update::att_update(EKF_STA &pre,EKF_STA &cur)
{
    Vector3f midvel;Vector3d midpos;
    midvel = (pre.vel + cur.vel) *0.5;;

    Quaterniond qne_pre, qne_cur;
    qne_pre   = earth::get_quaterniond_n2e_from_pos(pre.pos),
    qne_cur   = earth::get_quaterniond_n2e_from_pos(cur.pos);

    Quaterniond q_t;
    q_t=qne_cur.get_inverse() * qne_pre;

    Vector3f   t1;
    t1= Rotation::quaternion2rotvec(q_t);
    Quaterniond qne_mid;

    t1=t1*0.5;

    qne_mid   = qne_pre * Rotation::rotvec2quaternion(t1).get_inverse();
    midpos[2] = (cur.pos[2] + pre.pos[2])*0.5;
    midpos    = earth::get_position_from_qn2e(qne_mid, midpos[2]);

    float local_r[2];
    earth::get_local_Radius_from_latitude(midpos[0],local_r);
    Vector3f wei_n,wne_n;
    wei_n=earth::get_wei_n(midpos[0]),
    wne_n=earth::get_wne_n(local_r,midpos,midvel);

    t1 = -(wei_n + wne_n) * dt;
    Quaterniond qnn,qbb;
    qnn= Rotation::rotvec2quaternion(t1);

    t1 = cur.imu.dtheta + pre.imu.dtheta.cross(cur.imu.dtheta) *(1/ 12);
    qbb   = Rotation::rotvec2quaternion(t1);

    cur.att.a_quaterniond   = qnn * pre.att.a_quaterniond * qbb;
    cur.att.a_matrix   = Rotation::quaternion2matrix(cur.att.a_quaterniond);
    cur.att.a_euler = Rotation::matrix2euler(cur.att.a_matrix);

}

void my_update::data_integrate(IMU_DATA &imu_data,IMU_HANDLED &imu)
{
    imu.dvel[0]=imu_data.acc_x*dt;
    imu.dvel[1]=imu_data.acc_y*dt;
    imu.dvel[2]=imu_data.acc_z*dt;

    imu.dtheta[0]=imu_data.gyro_x*dt;
    imu.dtheta[1]=imu_data.gyro_y*dt;
    imu.dtheta[2]=imu_data.gyro_z*dt;
}


void my_update::data_output(EKF_STA &sta,POS_DATA &pos, VEL_DATA &vel,ATT_DATA &att)
{
    pos.latitude=(180.0/3.1415926535)*sta.pos[0];
    pos.longitude=(180.0/3.1415926535)*sta.pos[1];
    pos.height=sta.pos[2];

    vel.vel_x=sta.vel[0];
    vel.vel_y=sta.vel[1];
    vel.vel_z=sta.vel[2];

    att.pitch=(180.0/3.1415926535)*sta.att.a_euler[0];
    att.roll=(180.0/3.1415926535)*sta.att.a_euler[1];
    att.yaw=(180.0/3.1415926535)*sta.att.a_euler[2];


}


void my_update::imu_compensate(IMU_HANDLED &imu,IMU_ERROR &imu_error)
{
      // 补偿IMU零偏误差
    // compensate the imu bias error
    imu.dtheta -= imu_error.gyrbias * dt;

    imu.dvel    -= imu_error.accbias * dt;

    // 补偿IMU比例因子误差
    // compensate the imu scale error


    Vector3f vector_t;
    vector_t.set_Ones();

    Vector3f gyrscale, accscale;
    gyrscale    = vector_t + imu_error.gyrscale;
    accscale    = vector_t + imu_error.accscale;
    imu.dtheta = imu.dtheta.get_cwiseProduct(gyrscale.get_cwiseInverse());
    imu.dvel   = imu.dvel.get_cwiseProduct(accscale.get_cwiseInverse());


}




void my_update::update(EKF_STA &pre,EKF_STA &cur)//机械编排算法
{
    my_update::vel_update(pre,cur);
    my_update::pos_update(pre,cur);
    my_update::att_update(pre,cur);
}



