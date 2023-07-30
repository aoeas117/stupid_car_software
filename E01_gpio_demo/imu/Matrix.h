/*
 * Matrix.h
 *
 *  Created on: 2023年4月9日
 *      Author: fuzic
 */

#ifndef IMU_MATRIX_H_
#define IMU_MATRIX_H_

#include "My_header.h"


class MatrixXf
{
        public:
    unsigned char row;
    unsigned char column;
    float* matrix;
public:
    MatrixXf(unsigned char x, unsigned char y) :row(x), column(y)
    {
        matrix = new float[x * y];
        for (unsigned char i = 0; i < x; i++)
            for (unsigned char j = 0; j < y; j++)
                matrix[i * y + j] = 0;

    }
    MatrixXf(MatrixXf& m) :row(m.row), column(m.column)
    {

        matrix = new float[m.row * m.column];
        for (unsigned char i = 0; i < row; i++)
            for (unsigned char j = 0; j < column; j++)
                matrix[i * column + j] = m.matrix[i * column + j];

    }

    MatrixXf():row(0),column(0)
    {
        matrix=0;
    }
    ~MatrixXf()
    {
        delete[]matrix;

    }

public:
    MatrixXf operator + (const MatrixXf& m) const
    {
        if (row == m.row && column == m.column)
        {
            MatrixXf r(row, column);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    r.matrix[i * column + j] = m.matrix[i * column + j] + matrix[i * column + j];
            return r;
        }

    }
    MatrixXf operator - (const MatrixXf& m) const
    {
        if (row == m.row && column == m.column)
        {
            MatrixXf r(row, column);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    r.matrix[i * column + j] = matrix[i * column + j] - m.matrix[i * column + j];
            return r;
        }

    }

    MatrixXf operator - (void) const
    {
            MatrixXf r(row, column);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    r.matrix[i * column + j] = -matrix[i * column + j];
            return r;
    }



    MatrixXf operator * (const MatrixXf& m) const
    {
        if (column == m.row)
        {
            MatrixXf r(row, m.column);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char k = 0; k < column; k++)
                {
                    float s= matrix[i * column + k];
                    for(unsigned char j = 0; j < m.column; j++)
                        r.matrix[i * m.column + j] += (s *m.matrix[k * m.column + j]);
                }
            return r;
        }
    }

    MatrixXf operator * (const float& n) const
    {
        MatrixXf r(row, column);
        for (unsigned char i = 0; i < row; i++)
            for (unsigned char j = 0; j < column; j++)
                r.matrix[i * column + j] = matrix[i * column + j] * n;
        return r;
    }


    MatrixXf operator = (const MatrixXf& m)
    {
        if (row == m.row && column == m.column)
        {
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    matrix[i * column + j] = m.matrix[i * column + j];
            return *this;
        }

    }
    float& operator () (unsigned char n)
    {
        if (row == 1 || column == 1)
            return matrix[n];
    }
    float* operator [] (unsigned char n)
    {
        if (n < row)
            return matrix + n * column;
    }
public:
    void set_Zero(void)
    {
        for (unsigned char i = 0; i < row; i++)
            for (unsigned char j = 0; j < column; j++)
                matrix[i * column + j] = 0;
    }

    MatrixXf get_Zero(void)
    {
        MatrixXf r(row, column);
        for (unsigned char i = 0; i < row; i++)
            for (unsigned char j = 0; j < column; j++)
                r.matrix[i * column + j] = 0;
        return r;

    }

    void set_Identity(void)
    {
        if (row == column)
        {
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                {
                    if (i == j)
                        matrix[i * column + j] = 1;
                    else
                        matrix[i * column + j] = 0;
                }
        }
    }

    MatrixXf get_Identity(void)
    {
        if (row == column)
        {
            MatrixXf r(row, column);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                {
                    if (i == j)
                        r.matrix[i * column + j] = 1;
                    else
                        r.matrix[i * column + j] = 0;
                }
            return r;
        }

    }


    void Add_Identity(void)
    {
        if (row == column)
        {
            for (unsigned char i = 0; i < row; i++)
               matrix[i*column+i]+=1;
        }

    }

    void Transpose(void)
    {
        if (row == column)
        {
            float t;
            for (unsigned char i = 0; i < row - 1; i++)
                for (unsigned char j = i + 1; j < column; j++)
                {
                    t = matrix[i * column + j];
                    matrix[i * column + j] = matrix[j * column + i];
                    matrix[j * column + i] = t;
                }
        }
        else
        {
            MatrixXf t(column, row);
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    t.matrix[j* row + i] = matrix[i* column + j];
            unsigned char temp;
            temp = row, row = column, column = temp;
            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    matrix[i * column + j] = t.matrix[i * column+j];
        }

    }

    MatrixXf get_Transpose(void)
    {
        MatrixXf r(column, row);
        for (unsigned char i = 0; i < row; i++)
            for (unsigned char j = 0; j < column; j++)
                r.matrix[j * row + i] = matrix[i * column + j];
        return r;

    }

    void Inverse()
    {
        if (row == column)
        {
            MatrixXf t(row, column);
            MatrixXf Cofactor(row - 1, row - 1);

            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                {
                    Cofactor = get_Cofactor(*this, i, j);
                    t.matrix[j * column + i] = (1 - 2 * ((i + j) % 2)) * determinant(Cofactor);
                }
            t = t * (1 / determinant(*this));

            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                    matrix[i * column + j] = t.matrix[i * column + j];


        }
    }

    MatrixXf get_Inverse()
    {
        if (row == column)
        {
            MatrixXf r(row, column);
            MatrixXf Cofactor(row - 1, row - 1);

            for (unsigned char i = 0; i < row; i++)
                for (unsigned char j = 0; j < column; j++)
                {
                    Cofactor = get_Cofactor(*this, i, j);
                    r.matrix[j * column + i] = (1 - 2 * ((i + j) % 2)) * determinant(Cofactor);
                }
            r = r * (1 / determinant(*this));

            return r;
        }

    }


    void set_Block_Matrix(MatrixXf &m, unsigned char r, unsigned char c)
    {
        if (m.row + r <= row || m.column + c <= column)
        {
            for (unsigned char i = 0; i < m.row; i++)
                for (unsigned char j = 0; j < m.column; j++)
                    matrix[(r + i) * column + j + c] = m.matrix[i * m.column + j];
        }
    }


    MatrixXf get_Block_Matrix(unsigned char r, unsigned char c, unsigned char h, unsigned char w)
    {

        if (r + h <= row || c + w <= column)
        {
            MatrixXf m(h, w);
            for (unsigned char i = 0; i < h; i++)
                for (unsigned char j = 0; j < w; j++)
                    m.matrix[i * w + j] = matrix[(r + i) * column + j + c];
            return m;
        }

    }


    float get_matrix_trace()
    {

        if (row == column)
        {
            float t=0;
            for (unsigned char i = 0; i < row; i++)
                t += matrix[i * column + i];
            return t;
        }
    }



    public:
        static MatrixXf get_Cofactor(MatrixXf& m, unsigned char r, unsigned char c)//求伴随矩阵
        {
            if (m.row == m.column && r < m.row && c < m.column)
            {
                MatrixXf cofactor(m.row - 1, m.row - 1);

                unsigned char x, y;
                for (unsigned char i = 0; i < m.row - 1; i++)
                    for (unsigned char j = 0; j < m.row - 1; j++)
                    {
                        if (i >= r && r != m.row)
                            x = i + 1;
                        else
                            x = i;

                        if (j >= c && c != m.row)
                            y = j + 1;
                        else
                            y = j;

                        cofactor.matrix[i * cofactor.column + j] = m.matrix[x * m.column + y];
                    }
                return cofactor;
            }
        }



        static float determinant(MatrixXf& m)//求行列式
        {

            if (m.row == m.column)
            {
                float result = 0, cofactor = 0;
                short sign = 1;
                MatrixXf Cofactor(m.row - 1, m.row - 1);
                if (m.row == 1)
                    result = m.matrix[0];
                else if (m.row == 2)
                {
                    result = m.matrix[0] * m.matrix[3] - m.matrix[1] * m.matrix[2];
                }
                else
                {
                    for (unsigned char i = 0; i < m.row; i++)
                    {
                        cofactor = m.matrix[i * m.column];
                        if (cofactor != 0)
                        {
                            Cofactor = get_Cofactor(m, i, 0);
                            result += sign * determinant(Cofactor) * cofactor;
                        }
                        sign *= -1;
                    }
                }
                return result;
            }
        }






};




#endif /* IMU_MATRIX_H_ */
