/*
 * Transformer.h
 *
 *  Created on: 2023Äê4ÔÂ9ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_TRANSFORMER_H_
#define IMU_TRANSFORMER_H_

#include "My_header.h"



class
    Rotation

{

#define M_PI 3.14159265358979323846;

public:
    static Vector3f matrix2euler(MatrixXf& dcm)
    {
        Vector3f euler;

        euler[1] = matan(-dcm[2][0] / sqrt(dcm[2][1] * dcm[2][1] + dcm[2][2] * dcm[2][2]));

        if (dcm[2][0] <= -0.999) {
            euler[0] = matan2(dcm[2][1], dcm[2][2]);
            euler[2] = matan2((dcm[1][2] - dcm[0][1]), (dcm[0][2] + dcm[1][1]));
        }
        else if (dcm[2][0] >= 0.999) {
            euler[0] = matan2(dcm[2][1], dcm[2][2]);
            euler[2] = (3.14159265358979323846 + matan2((dcm[1][2] + dcm[0][1]), (dcm[0][2] - dcm[1][1])));
        }
        else {
            euler[0] = matan2(dcm[2][1], dcm[2][2]);
            euler[2] = matan2(dcm[1][0], dcm[0][0]);
        }

        // heading 0~2PI
        if (euler[2] < 0) {
            euler[2] = 2 * 3.14159265358979323846 + euler[2];
        }

        return euler;
    }

    static MatrixXf euler2matrix(Vector3f& v)
    {
        MatrixXf r(3, 3);
        r[0][0] = cos(v[2]) * cos(v[1]), r[0][1] = -sin(v[2]) * cos(v[0]) + cos(v[2]) * sin(v[1]) * sin(v[0]), r[0][2] = sin(v[2]) * sin(v[0]) + cos(v[2]) * sin(v[1]) * cos(v[0]);
        r[1][0] = sin(v[2]) * cos(v[1]), r[1][1] = cos(v[2]) * cos(v[0]) + sin(v[1]) * sin(v[0])*sin(v[2]), r[1][2] = -cos(v[2]) * sin(v[0]) + sin(v[1]) * sin(v[2]) * cos(v[0]);
        r[2][0] = -sin(v[1]), r[2][1] = cos(v[1]) * sin(v[0]), r[2][2] = cos(v[1]) * cos(v[0]);
        return r;
    }


    static Quaterniond  euler2quaternion(Vector3f& v)
    {
        Quaterniond r;

        r.w = cos(v[2] / 2) * cos(v[1] / 2) * cos(v[0] / 2) + sin(v[2] / 2) * sin(v[1] / 2) * sin(v[0] / 2);
        r.z = sin(v[2] / 2) * cos(v[1] / 2) * cos(v[0] / 2) - cos(v[2] / 2) * sin(v[1] / 2) * sin(v[0] / 2);
        r.y = cos(v[2] / 2) * sin(v[1] / 2) * cos(v[0] / 2) + sin(v[2] / 2) * cos(v[1] / 2) * sin(v[0] / 2);
        r.x = cos(v[2] / 2) * cos(v[1] / 2) * sin(v[0] / 2) - sin(v[2] / 2) * sin(v[1] / 2) * cos(v[0] / 2);

        return r;
    }


    static MatrixXf quaternion2matrix(Quaterniond& q)
    {
        MatrixXf r(3, 3);

        r[0][0] = q.w * q.w + q.x * q.x - q.y * q.y - q.z * q.z, r[0][1] = 2 * (q.x * q.y - q.w * q.z), r[0][2] = 2 * (q.x * q.z + q.w * q.y);
        r[1][0] = 2 * (q.x * q.y + q.w * q.z), r[1][1] = q.w * q.w - q.x * q.x + q.y * q.y - q.z * q.z, r[1][2] = 2 * (q.y * q.z - q.w * q.x);
        r[2][0] = 2 * (q.x * q.z - q.w * q.y), r[2][1] = 2 * (q.y * q.z + q.w * q.x), r[2][2] = q.w * q.w - q.x * q.x - q.y * q.y + q.z * q.z;

        return r;
    }

    static Vector3f quaternion2rotvec(Quaterniond& q)
    {
        Vector3f r;
        if (q.w != 0)
        {
            float t = matan2(sqrt(q.x * q.x + q.y * q.y + q.z * q.z), q.w);
            r = q.get_vec() * ((2 * t) / sin(t));
        }
        else
            r = q.get_vec() * M_PI;
        return r;
    }


    static Quaterniond rotvec2quaternion(Vector3f& rotvec)
    {
        Vector3f v;
        v = rotvec * 0.5;
        Quaterniond q;
        float t = v.norm();
        if(t==0)
        {
            q.w=1;
            return q;
        }

        q.w = cos(t);
        v = v * (sin(t) / t);
        q.x = v[0];
        q.y = v[1];
        q.z = v[2];
        return q;
    }

    static MatrixXf rotvec2matrix(Vector3f& v)
    {
        float t = v.norm();

        MatrixXf m(3, 3);

        m = v.get_Antisymmetric_Matrix();

        m = m.get_Identity() + m * (sin(t) / t) + m * m * ((1 - cos(t) )/ (t * t));

        return m;



    }

    static Quaterniond matrix2quaternion(MatrixXf& m)
    {
        float t[4];
        t[0] = 1 + m.get_matrix_trace();
        t[1] = 1 + 2 * m[0][0] - m.get_matrix_trace();
        t[2] = 1 + 2 * m[1][1] - m.get_matrix_trace();
        t[3] = 1 + 2 * m[2][2] - m.get_matrix_trace();


        Quaterniond q;


        if (t[0] >= t[1] && t[0] >= t[2] && t[0] >= t[3])
        {
            q.w = sqrt(t[0]) / 2; q.x = (m[2][1] - m[1][2]) / (4 * q.w);
            q.y = (m[0][2] - m[2][0]) / (4 * q.w); q.z = (m[1][0] - m[0][1]) / (4 * q.w);
        }
        else if (t[1] >= t[0] && t[1] >= t[2] && t[1] >= t[3])
        {
            q.x = sqrt(t[1]) / 2; q.y = (m[1][0] + m[0][1]) / (4 * q.x);
            q.z = (m[0][2] + m[2][0]) / (4 * q.x); q.w = (m[2][1] - m[1][2]) / (4 * q.x);
        }
        else if (t[2] >= t[1] && t[2] >= t[0] && t[2] >= t[3])
        {
            q.y = sqrt(t[2]) / 2; q.z = (m[2][1] + m[1][2]) / (4 * q.y);
            q.w = (m[0][2] - m[2][0]) / (4 * q.y); q.x = (m[0][1] + m[1][0]) / (4 * q.y);
        }
        else
        {
            q.z = sqrt(t[3]) / 2; q.w = (m[1][0] - m[0][1]) / (4 * q.z);
            q.x = (m[0][2] + m[2][0]) / (4 * q.z); q.y = (m[2][1] + m[1][2]) / (4 * q.z);

        }
        return q;

    }

    static Vector3f matrix2vector3d(MatrixXf &m)
    {
        if((m.column==3 && m.row==1)||(m.row==3 && m.column==1))
        {
            Vector3f r;
            r[0]=m.matrix[0];
            r[1]=m.matrix[1];
            r[2]=m.matrix[2];
            return r;
        }
    }

    static float deg2rad(float t)
    {
        return  (t / 180)*M_PI;

    }


    static float rad2deg(float t)
    {
        return t * 180 / M_PI;
    }

    static float get_g(float t)
    {
        return 9.7803267715 * t;
    }


};







#endif /* IMU_TRANSFORMER_H_ */
