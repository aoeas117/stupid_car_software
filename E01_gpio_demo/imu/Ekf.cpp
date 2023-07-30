/*
 * Ekf.cpp
 *
 *  Created on: 2023年4月20日
 *      Author: fuzic
 */
#include "Ekf.h"


EKF_UPDATE ekf_update;

STA_STD ekf_sta_std;

ImuNoise imu_noise;

Vector3f install_l;

GNSS_DATA gnss;







void ekf::ekf_update_init(EKF_UPDATE &ekf_update,STA_STD &sta_std,ImuNoise &imu_noise)
{
    //ekf_update.cov.resize(21,21);
    //ekf_update.dx.resize(21,1);
    //ekf_update.qc.resize(18,18);

//    ekf_update.cov.setZero();
//    ekf_update.dx.setZero();
//    ekf_update.qc.setZero();

    MatrixXf temp_m(3,3);

    temp_m=sta_std.pos.get_cwiseProduct(sta_std.pos).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 0, 0);

    temp_m=sta_std.vel.get_cwiseProduct(sta_std.vel).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 3, 3);

    temp_m=sta_std.euler.get_cwiseProduct(sta_std.euler).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 6, 6);

    temp_m=sta_std.gyrbias.get_cwiseProduct(sta_std.gyrbias).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 9, 9);

    temp_m=sta_std.accbias.get_cwiseProduct(sta_std.accbias).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 12, 12);

    temp_m=sta_std.gyrscale.get_cwiseProduct(sta_std.gyrscale).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 15, 15);

    temp_m=sta_std.accscale.get_cwiseProduct(sta_std.accscale).get_Asdiagonal();
    ekf_update.cov.set_Block_Matrix(temp_m, 18, 18);


    temp_m=imu_noise.acc_vrw.get_cwiseProduct(imu_noise.acc_vrw).get_Asdiagonal();
    ekf_update.qc.set_Block_Matrix(temp_m, 0, 0);

    temp_m=imu_noise.gyr_arw.get_cwiseProduct(imu_noise.gyr_arw).get_Asdiagonal();
    ekf_update.qc.set_Block_Matrix(temp_m, 3, 3);

    temp_m=imu_noise.gyrbias_std.get_cwiseProduct(imu_noise.gyrbias_std).get_Asdiagonal()*(2 / corr_time);
    ekf_update.qc.set_Block_Matrix(temp_m, 6, 6);

    temp_m=imu_noise.accbias_std.get_cwiseProduct(imu_noise.accbias_std).get_Asdiagonal()*(2 / corr_time);
    ekf_update.qc.set_Block_Matrix(temp_m, 9, 9);

    temp_m=imu_noise.gyrscale_std.get_cwiseProduct(imu_noise.gyrscale_std).get_Asdiagonal()*(2 / corr_time);
    ekf_update.qc.set_Block_Matrix(temp_m, 12, 12);

    temp_m=imu_noise.accscale_std.get_cwiseProduct(imu_noise.accscale_std).get_Asdiagonal()*(2 / corr_time);
    ekf_update.qc.set_Block_Matrix(temp_m, 15, 15);



}




MatrixXf ekf::get_sta_transform_matrix(EKF_STA &presta,IMU_DATA &cur_imu)
{

    MatrixXf F(21, 21); MatrixXf temp_v(3, 1);
       //F.setZero();

        float local_r[2];
       earth::get_local_Radius_from_latitude(presta.pos[0], local_r);
       float   gravity = earth::get_local_gravity_from_pos(presta.pos);//当地重力
       Vector3f wei_n, wne_n;
       wei_n = earth::get_wei_n(presta.pos[0]), wne_n = earth::get_wne_n(local_r, presta.pos, presta.vel);

       Vector3f accel, palst;
       float rmh = local_r[0] + presta.pos[2], rnh = local_r[1] + presta.pos[2];

       accel[0] = cur_imu.acc_x, accel[1] = cur_imu.acc_y, accel[2] = cur_imu.acc_z;//加速度
       palst[0] = cur_imu.gyro_x, palst[1] = cur_imu.gyro_y, palst[2] = cur_imu.gyro_z;//角速度

//
       // 位置误差
       // position error
       MatrixXf temp(3, 3);
       temp[0][0] = -presta.vel[2] / rmh;
       temp[0][2] = presta.vel[0] / rmh;
       temp[1][0] = presta.vel[1] * tan(presta.pos[0]) / rnh;
       temp[1][1] = -(presta.vel[2] + presta.vel[0] * tan(presta.pos[0])) / rnh;
       temp[1][2] = presta.vel[1] / rnh;
       F.set_Block_Matrix(temp, 0, 0);
       temp.set_Identity();
       F.set_Block_Matrix(temp, 0, 3);



       // 速度误差
       // velocity error
       temp.set_Zero();
       temp[0][0] = (-2 * presta.vel[1] * 7.2921151467E-5 * cos(presta.pos[0])) / rmh -
           mpow(presta.vel[1], 2) / (rmh * rnh * mpow(cos(presta.pos[0]), 2));
       temp[0][2] = presta.vel[0] * presta.vel[2] / mpow(rmh, 2) - (mpow(presta.vel[1], 2) * tan(presta.pos[0])) / mpow(rnh, 2);

       temp[1][0] = 2 * 7.2921151467E-5 * (presta.vel[0] * cos(presta.pos[0]) - presta.vel[2] * sin(presta.pos[0])) / rmh +
           presta.vel[0] * presta.vel[1] / (rmh * rnh * mpow(cos(presta.pos[0]), 2));
       temp[1][2] = (presta.vel[1] * presta.vel[2] + presta.vel[0] * presta.vel[1] * tan(presta.pos[0])) / mpow(rnh, 2);
       temp[2][0] = 2 * 7.2921151467E-5 * presta.vel[1] * sin(presta.pos[0]) / rmh;
       temp[2][2] = -mpow(presta.vel[1], 2) / mpow(rnh, 2) - mpow(presta.vel[0], 2) / mpow(rmh, 2) + 2 * gravity / (msqrt(local_r[0] * local_r[1]) + presta.pos[2]);
       F.set_Block_Matrix(temp, 3, 0);


       temp.set_Zero();
       temp[0][0] = presta.vel[2] / rmh;
       temp[0][1] = -2 * (7.2921151467E-5 * sin(presta.pos[0]) + presta.vel[1] * tan(presta.pos[0]) / rnh);
       temp[0][2] = presta.vel[0] / rmh;
       temp[1][0] = 2 * 7.2921151467E-5 * sin(presta.pos[0]) + presta.vel[1] * tan(presta.pos[0]) / rnh;
       temp[1][1] = (presta.vel[2] + presta.vel[0] * tan(presta.pos[0])) / rnh;
       temp[1][2] = 2 * 7.2921151467E-5 * cos(presta.pos[0]) + presta.vel[1] / rnh;
       temp[2][0] = -2 * presta.vel[0] / rmh;
       temp[2][1] = -2 * (7.2921151467E-5 * cos(presta.pos[0]) + presta.vel[1] / rnh);
       F.set_Block_Matrix(temp, 3, 3);
       temp_v = presta.att.a_matrix * accel.vector_column();
       temp = Rotation::matrix2vector3d(temp_v).get_Antisymmetric_Matrix();
       F.set_Block_Matrix(temp, 3, 6);
       F.set_Block_Matrix(presta.att.a_matrix, 3, 12);
       temp = presta.att.a_matrix * (accel.get_Asdiagonal());
       F.set_Block_Matrix(temp, 3, 18);



       //姿态误差
       temp.set_Zero();
       temp[0][0] = -7.2921151467E-5 * sin(presta.pos[0]) / rmh;
       temp[0][2] = presta.vel[1] / mpow(rnh, 2);
       temp[1][2] = -presta.vel[0] / mpow(rmh, 2);
       temp[2][0] = -7.2921151467E-5 * cos(presta.pos[0]) / rmh - presta.vel[1] / (rmh * rnh * mpow(cos(presta.pos[0]), 2));
       temp[2][2] = -presta.vel[1] * tan(presta.pos[0]) / mpow(rnh, 2);
       F.set_Block_Matrix(temp, 6, 0);

       temp.set_Zero();
       temp[0][1] = 1 / rnh;
       temp[1][0] = -1 / rmh;
       temp[2][1] = -tan(presta.pos[0]) / rnh;
       F.set_Block_Matrix(temp, 6, 3);
       temp = -(wei_n + wne_n).get_Antisymmetric_Matrix();
       F.set_Block_Matrix(temp, 6, 6);
       temp = -presta.att.a_matrix;
       F.set_Block_Matrix(temp, 6, 9);

       temp = -presta.att.a_matrix * palst.get_Asdiagonal();
       F.set_Block_Matrix(temp, 6, 15);



       temp.set_Identity();
       temp = temp * (-1.0 / corr_time);

       F.set_Block_Matrix(temp, 9, 9);
       F.set_Block_Matrix(temp, 12, 12);
       F.set_Block_Matrix(temp, 15, 15);
       F.set_Block_Matrix(temp, 18, 18);

       F =  F * dt;
       F.Add_Identity();



        return F;

}




MatrixXf ekf::get_noise_drive_matrix(MatrixXf &a_matrix)
{
   MatrixXf G(21, 18);
   G.set_Block_Matrix(a_matrix, 3, 0);
   G.set_Block_Matrix(a_matrix, 6, 3);
   MatrixXf m(3, 3);
   m.set_Identity();
   G.set_Block_Matrix(m, 9, 6);
   G.set_Block_Matrix(m, 12, 9);
   G.set_Block_Matrix(m, 15, 12);
   G.set_Block_Matrix(m, 18, 15);

    return G;

}

void ekf::ekf_predict(MatrixXf &phi, MatrixXf &qd,EKF_UPDATE &ekf_update)
{

    // 传播系统协方差和误差状态

    ekf_update.cov = phi * ekf_update.cov * phi.get_Transpose() + qd;
    ekf_update.dx  = phi * ekf_update.dx;



}


void ekf::ekf_update(MatrixXf &dz,MatrixXf &H,MatrixXf &R,EKF_UPDATE &ekf_update)
{
    // 计算Kalman增益
    // Compute Kalman Gain
    MatrixXf temp(3, 3);
    temp = H * ekf_update.cov * H.get_Transpose() + R;//R阵是观测向量的协方差阵(3*3)

    MatrixXf K(21,3);
    K = ekf_update.cov * H.get_Transpose() * temp.get_Inverse();

    // 更新系统误差状态和协方差
    // update system error state and covariance
    MatrixXf I(21, 21);
    I.set_Identity();
    I = I - K * H;
    // 如果每次更新后都进行状态反馈，则更新前dx_一直为0，下式可以简化为：dx_ = K * dz;
    // if state feedback is performed after every update, dx_ is always zero before the update
    // the following formula can be simplified as : dx_ = K * dz;
    ekf_update.dx =  K * dz;
    ekf_update.cov = I * ekf_update.cov * I.get_Transpose() + K * R * K.get_Transpose();
}


void ekf::ekf_gps_update(GNSS_DATA &gnss_data,Vector3f l,EKF_STA &presta,EKF_UPDATE &ekf_update)
{

    MatrixXf n2geo(3, 3), geo2n(3, 3), temp_v(3, 1), temp(3, 3);
    n2geo = earth::get_matrix_n2geo_pos(presta.pos),
    geo2n = earth::get_matrix_geo2n_pos(presta.pos);

    Vector3f antenna_pos;
    temp_v = n2geo * presta.att.a_matrix * l.vector_column();;
    Vector3f temp_v3f;
    temp_v3f=Rotation::matrix2vector3d(temp_v);

    antenna_pos[0] = presta.pos[0] + temp_v3f[0];antenna_pos[1] = presta.pos[1] + temp_v3f[1];antenna_pos[2] = presta.pos[2] + temp_v3f[2];
    MatrixXf dz(3, 1);
    dz = geo2n * (antenna_pos - gnss_data.pos).vector_column();

    MatrixXf H(3, 21);
    temp.set_Identity();
    H.set_Block_Matrix(temp, 0, 0);
    temp_v = presta.att.a_matrix * l.vector_column();
    temp = Rotation::matrix2vector3d(temp_v).get_Antisymmetric_Matrix();
    H.set_Block_Matrix(temp, 0, 6);

    MatrixXf R(3, 3);
    R = gnss_data.error.get_cwiseProduct(gnss_data.error).get_Asdiagonal();//外部读取的测量经度作为观测误差

    ekf::ekf_update(dz, H, R, ekf_update);



}


void ekf::ekf_feedback(EKF_UPDATE &ekf_update,EKF_STA &cursta)
{
    MatrixXf vectemp(3,1);Vector3f v_t;
    MatrixXf matrixtemp(3,3);
    //位置误差
    vectemp = ekf_update.dx.get_Block_Matrix(0, 0, 3, 1);

    matrixtemp  = earth::get_matrix_n2geo_pos(cursta.pos);
    vectemp=matrixtemp* vectemp;

    Vector3f temp_v3f;
    temp_v3f=Rotation::matrix2vector3d(vectemp);

    cursta.pos[0] = cursta.pos[0] - temp_v3f[0];
    cursta.pos[1] = cursta.pos[1] - temp_v3f[1];
    cursta.pos[2] = cursta.pos[2] - temp_v3f[2];


    vectemp = ekf_update.dx.get_Block_Matrix(3, 0, 3, 1);
    cursta.vel =cursta.vel- Rotation::matrix2vector3d(vectemp);

    // 姿态误差反馈
    // attitude error feedback
    vectemp                         = ekf_update.dx.get_Block_Matrix(6, 0, 3, 1);
    v_t=Rotation::matrix2vector3d(vectemp);
    if(v_t.norm()<0.001)
    {

        Quaterniond qpn                 = Rotation::rotvec2quaternion(v_t);
        cursta.att.a_quaterniond        = qpn * cursta.att.a_quaterniond;
        cursta.att.a_matrix             = Rotation::quaternion2matrix(cursta.att.a_quaterniond);
        cursta.att.a_euler              = Rotation::matrix2euler(cursta.att.a_matrix);
    }
    // IMU零偏误差反馈
    // IMU bias error feedback
    vectemp = ekf_update.dx.get_Block_Matrix(9, 0, 3, 1);
    ekf_update.imu_error.gyrbias += Rotation::matrix2vector3d(vectemp);
    vectemp = ekf_update.dx.get_Block_Matrix(12, 0, 3, 1);
    ekf_update.imu_error.accbias += Rotation::matrix2vector3d(vectemp);

    // IMU比例因子误差反馈
    // IMU sacle error feedback
    vectemp = ekf_update.dx.get_Block_Matrix(15, 0, 3, 1);
    ekf_update.imu_error.gyrscale += Rotation::matrix2vector3d(vectemp);
    vectemp = ekf_update.dx.get_Block_Matrix(18, 0, 3, 1);;
    ekf_update.imu_error.accscale += Rotation::matrix2vector3d(vectemp);

    // 误差状态反馈到系统状态后,将误差状态清零
    // set 'dx' to zero after feedback error state to system state
    ekf_update.dx.set_Zero();

}


void ekf::gnss_data_copy(GPS_DATA &gps_dat,GNSS_DATA &gnss_dat)
{
    gnss_dat.pos[0]=(PI/180.0)*gps_dat.latitude,gnss_dat.pos[1]=(PI/180.0)*gps_dat.longitude,gnss_dat.pos[2]=gps_dat.height;
    gnss_dat.error[0]=gps_dat.hoop_x,gnss_dat.error[1]=gps_dat.hoop_y,gnss_dat.error[2]=gps_dat.hoop_z;

}









MatrixXf PhixQd(MatrixXf& F, MatrixXf& qd)
{
    MatrixXf r(21, 21);
    MatrixXf m1(3, 3), m2(3, 3),m(3,3);


    m2 = qd.get_Block_Matrix(3, 3, 3, 3);
    for (unsigned char i = 0; i < 3; i++)
    {
        m1 = F.get_Block_Matrix(i*3, 3, 3, 3);
        m = m1 * m2;
        r.set_Block_Matrix(m, i*3, 3);
    }
    m2 = qd.get_Block_Matrix(6, 6, 3, 3);

    m1 = F.get_Block_Matrix(3, 6, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 3, 6);

    m1 = F.get_Block_Matrix(6, 6, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 6, 6);

    for (unsigned char i = 0; i < 4; i++)
    {
        m1 = F.get_Block_Matrix(9 + i * 3, 9 + i * 3, 3, 3);
        m2 = qd.get_Block_Matrix(9 + i * 3, 9 + i * 3, 3, 3);
        m = m1 * m2;
        r.set_Block_Matrix(m, 9 + i * 3, 9 + i * 3);
    }

    m1 = F.get_Block_Matrix(6, 9, 3, 3);
    m2 = qd.get_Block_Matrix(9, 9, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 6, 9);

    m1 = F.get_Block_Matrix(3, 12, 3, 3);
    m2 = qd.get_Block_Matrix(12, 12, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 3, 12);

    m1 = F.get_Block_Matrix(6, 15, 3, 3);
    m2 = qd.get_Block_Matrix(15, 15, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 6, 15);

    m1 = F.get_Block_Matrix(3, 18, 3, 3);
    m2 = qd.get_Block_Matrix(18, 18, 3, 3);
    m = m1 * m2;
    r.set_Block_Matrix(m, 3, 18);

//    MatrixXf Ft(21,21);
//    Ft=F.get_Transpose();
//
//    r=QdxPhi(r,Ft);


    return r;
}



void ekf::ekf_imu_update(EKF_STA &presta,EKF_STA &cursta,EKF_UPDATE &ekf_update,IMU_DATA &cur_imu)
{
    my_update::imu_compensate(cursta.imu,ekf_update.imu_error);

    my_update::update(presta,cursta);//姿态更新
//
//    MatrixXf phi(21,21),temp1(3,3),temp2(3,3);
//
//    phi=ekf::get_sta_transform_matrix(presta,cur_imu);//状态转移矩阵
//
//    MatrixXf G(21,18);
//    G=ekf::get_noise_drive_matrix(cursta.att.a_matrix);
//
//    MatrixXf Qd(21,21);
//
//    //Qd= G*q*G(t)
//
//    for (unsigned char i = 0; i < 6; i++)
//    {
//           temp1 = G.get_Block_Matrix((i + 1) * 3, i * 3, 3, 3);
//           temp2 = ekf_update.qc.get_Block_Matrix(i * 3, i * 3, 3, 3);
//           temp1 = temp1 * temp2 * temp1.get_Transpose()*dt;
//           Qd.set_Block_Matrix(temp1, (i + 1) * 3, (i + 1) * 3);
//    }
//    Qd = (PhixQd(phi,Qd)*phi.get_Transpose()+ Qd)*0.5;
//
//    ekf_predict(phi,Qd,ekf_update);//更新cov与dx矩阵


}






