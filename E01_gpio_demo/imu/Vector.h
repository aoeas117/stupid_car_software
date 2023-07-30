/*
 * Vector.h
 *
 *  Created on: 2023年4月9日
 *      Author: fuzic
 */

#ifndef IMU_VECTOR_H_
#define IMU_VECTOR_H_

#include "My_header.h"

template <typename T>

class Vector3
{

        public:
        T v[3];
    public:
        Vector3()
        {
            v[0] = 0;
            v[1] = 0;
            v[2] = 0;
        }
        Vector3(T x, T y, T z)
        {
            v[0] = x;
            v[1] = y;
            v[2] = z;
        }
        Vector3(Vector3& vector)
        {
            v[0] = vector[0];
            v[1] = vector[1];
            v[2] = vector[2];
        }

        ~Vector3() {};

    public:
        T& operator[](unsigned char n)
        {
            if (n < 3)
                return v[n];
            else
                return v[0];
        }


        Vector3 operator()(T x, T y, T z)
        {
            v[0] = x;
            v[1] = y;
            v[2] = z;
            return* this;
        }

        Vector3 operator = (const Vector3 &vector)
        {
            v[0] = vector.v[0];
            v[1] = vector.v[1];
            v[2] = vector.v[2];
            return *this;
        }
        Vector3 operator + (const Vector3& vector) const
        {
            Vector3 r;
            r[0] = vector.v[0] + v[0];
            r[1] = vector.v[1] + v[1];
            r[2] = vector.v[2] + v[2];
            return r;
        }

        Vector3 operator +=(const Vector3& vector)
        {
            v[0] += vector.v[0];
            v[1] += vector.v[1];
            v[2] += vector.v[2];
            return *this;
        }


        Vector3 operator - (const Vector3& vector) const
        {
            Vector3 r;
            r[0] = v[0] - vector.v[0];
            r[1] = v[1] - vector.v[1];
            r[2] = v[2] - vector.v[2];
            return r;
        }

        Vector3 operator - () const
        {
            Vector3 r;
            r[0] = -v[0];
            r[1] = -v[1];
            r[2] = -v[2];
            return r;

        }




        Vector3 operator -= (const Vector3& vector)
        {
            v[0] -= vector.v[0];
            v[1] -= vector.v[1];
            v[2] -= vector.v[2];
            return *this;
        }


        T    operator * (const Vector3& vector) const//点乘
        {
            T r = v[0] * vector.v[0] + v[1] * vector.v[1] + v[2] * vector.v[2];
            return r;
        }
        Vector3 operator * (const T& n) const //向量数乘
        {
            Vector3 r(v[0] * n, v[1] * n, v[2] * n);
            return r;
        }

    public:
        T norm(void)//取模
        {
            T r = sqrt(v[0] * v[0] + v[1] * v[1] + v[2] * v[2]);
            return r;
        }

        void normalize(void)//标准化
        {
            T t = this->norm();
            if ( t!= 0)
            {
                v[0] = v[0] / t;
                v[1] = v[1] / t;
                v[2] = v[2] / t;
            }
        }



        Vector3 get_normalize(void)//标准化
        {
            Vector3 r;
            T t = this->norm();
            if ( t!= 0)
            {
                r[0]=v[0]/t;
                r[1]=v[1]/t;
                r[2]=v[2]/t;
            }

            return r;
        }

        Vector3 cross(const Vector3& vector)
        {
            Vector3 r;
            r[0] = v[1] * vector.v[2] - v[2] * vector.v[1];
            r[1] = v[2] * vector.v[0] - v[0] * vector.v[2];
            r[2] = v[0] * vector.v[1] - v[1] * vector.v[0];
            return r;
        }

        Vector3 get_cwiseInverse()//各个位置上的倒数
        {
            Vector3 r;
            r[0]=1/v[0];
            r[1]=1/v[1];
            r[2]=1/v[2];

            return r;

        }


        Vector3 get_cwiseProduct(const Vector3& vector)//各个位置上的相乘
        {
            Vector3 r;
            r[0]=v[0]*vector.v[0];
            r[1]=v[1]*vector.v[1];
            r[2]=v[2]*vector.v[2];
            return r;
        }

        MatrixXf get_Asdiagonal(void)
        {
            MatrixXf r(3, 3);
            r[0][0]=v[0];
            r[1][1]=v[1];
            r[2][2]=v[2];
            return r;
        }

        MatrixXf get_Antisymmetric_Matrix(void)
        {
            MatrixXf r(3, 3);
            r[0][0] = 0, r[0][1] = -v[2], r[0][2] = v[1];
            r[1][0] = v[2], r[1][1] = 0, r[1][2] = -v[0];
            r[2][0] = -v[1], r[2][1] = v[0], r[2][2] = 0;
            return r;
        }



        void set_Zeros(void)
        {
            v[0]=0;
            v[1]=0;
            v[2]=0;
        }
        void set_Ones(void)
        {
            v[0]=1;
            v[1]=1;
            v[2]=1;
        }


        MatrixXf vector_row(void )//行向量
        {
            MatrixXf r(1, 3);
            r[0][0] = v[0], r[0][1] = v[1], r[0][2] = v[2];
            return r;
        }

        MatrixXf vector_column(void)//列向量
        {
            MatrixXf r(3, 1);
            r[0][0] = v[0], r[1][0] = v[1], r[2][0] = v[2];
            return r;

        }



};


typedef Vector3<float> Vector3f;
typedef Vector3<double> Vector3d;







#endif /* IMU_VECTOR_H_ */
