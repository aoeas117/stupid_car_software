/*
 * Quaterniond.h
 *
 *  Created on: 2023Äê4ÔÂ9ÈÕ
 *      Author: fuzic
 */

#ifndef IMU_QUATERNIOND_H_
#define IMU_QUATERNIOND_H_


#include "My_header.h"



class Quaterniond
{

    public:
        float w;
        float x;
        float y;
        float z;

    public:
        Quaterniond() :w(0), x(0), y(0), z(0) {}
        Quaterniond(float wi, float xi, float yi, float zi) :w(wi), x(xi), y(yi), z(zi) {}
        Quaterniond(const Quaterniond& q)
        {
            w = q.w;
            x = q.x;
            y = q.y;
            z = q.z;
        }


    public:
        Quaterniond operator =(const Quaterniond& q)
        {
            w = q.w;
            x = q.x;
            y = q.y;
            z = q.z;
            return *this;
        }

        Quaterniond operator + (const Quaterniond& q) const
        {
            Quaterniond r;
            r.w = w + q.w;
            r.x = x + q.x;
            r.y = y + q.y;
            r.z = z + q.z;
            return r;
        }

        Quaterniond operator - (const Quaterniond& q) const
        {
            Quaterniond r;
            r.w = w - q.w;
            r.x = x - q.x;
            r.y = y - q.y;
            r.z = z - q.z;
            return r;
        }
        Quaterniond operator - () const
        {
            Quaterniond r;
            r.w = -w;
            r.x = -x;
            r.y = -y;
            r.z = -z;
            return r;
        }




        Quaterniond operator * (const Quaterniond& q) const
        {
            Quaterniond r;

            r.w = w * q.w - x * q.x - y * q.y - z * q.z;
            r.x = w * q.x + x * q.w + y * q.z - z * q.y;
            r.y = w * q.y + y * q.w + z * q.x - x * q.z;
            r.z = w * q.z + z * q.w + x * q.y - y * q.x;

            return r;
        }


        Quaterniond operator * (const float n) const
        {
            Quaterniond r;
            r.w = n * w;
            r.x = n * x;
            r.y = n * y;
            r.z = n * z;
            return r;
        }



        public:
            float norm(void) const
            {
                float r;
                r = sqrt(w * w + x * x + y * y + z * z);
                return r;
            }

            void normalize(void)
            {
                float t = this->norm();
                if (t != 0)
                {
                    w = w / t;
                    x = x / t;
                    y = y / t;
                    z = z / t;
                }
            }

            Quaterniond get_normalize(void)
            {
                float t = this->norm();
                if (t != 0)
                {
                    Quaterniond q;
                    q.w = w / t;
                    q.x = x / t;
                    q.y = y / t;
                    q.z = z / t;
                    return q;
                }
            }

            void conjugate(void)
            {
                x = -x;
                y = -y;
                z = -z;
            }

            Quaterniond get_conjugate(void)
            {
                Quaterniond q;
                q.w = w;
                q.x = -x;
                q.y = -y;
                q.z = -z;
                return q;
            }
            void inverse(void)
            {
                float t = this->norm();

                *this = this->get_conjugate() * (1 / (t * t));
            }

            Quaterniond get_inverse(void)
            {
                float t = this->norm();
                Quaterniond q=this->get_conjugate()*(1/(t*t));
                return q;

            }

            Vector3f get_vec(void)
            {
                Vector3f r(x, y, z);
                return r;
            }


            float get_cos_half(void)
            {
                return  w / (this->norm());
            }

            float get_sin_half(void)
            {
                return sqrt(x * x + y * y + z * z) / (this->norm());
            }




};



#endif /* IMU_QUATERNIOND_H_ */
