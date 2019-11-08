/*
* easy_bo_tester - C ++ header-only library for testing binary options
*
* Copyright (c) 2018 Elektro Yar. Email: git.electroyar@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#ifndef EASY_BO_MATH_HPP_INCLUDED
#define EASY_BO_MATH_HPP_INCLUDED

#include <cmath>

namespace easy_bo_math {

    /** \brief Инверсный быстрый квадратный корень
     * \param x Число, из которого извлечем квадратный корень
     * \return Инверсный квадратный корень
     */
    inline float inv_sqrt(const float x) {
        const float halfx = 0.5f * x;
        float y = x;
        long i = *(long*)&y;
        i = 0x5f3759df - (i>>1);
        y = *(float*)&i;
        y = y * (1.5f - (halfx * y * y));
        return y;
    }

    const size_t SIN_COS_TABLE_SIZE = 1024; /**< Размер таблицы синусов и косинусов */
    static float sin_table[1024];           /**< Таблица синусов */
    static float cos_table[1024];           /**< Таблица косинусов */
    static bool is_generate_table = false;

    /** \brief Сгенерировать таблицу синусов и косинусов
     */
    void generate_table_sin_cos() {
        if(is_generate_table) return;
        const double PI = 3.1415926535897932384626433832795d;
        const double PI_DIV_2 = PI / 2.0d;
        for(uint32_t i = 0; i < SIN_COS_TABLE_SIZE; ++i) {
            sin_table[i] = std::sin(PI_DIV_2 * (double)i / (double)SIN_COS_TABLE_SIZE);
            cos_table[i] = std::cos(PI_DIV_2 * (double)i / (double)SIN_COS_TABLE_SIZE);
        }
        is_generate_table = true;
    }



    float get_sin(float angle) {
        const double PI = 3.1415926535897932384626433832795d;
        const double PI_DIV_2 = PI / 2.0d;
        const double PI_SUM_DIV_2 = PI + PI / 2.0d;
        const double PI_X2 = PI * 2.0d;
        const size_t MAX_INDEX = SIN_COS_TABLE_SIZE - 1;
        const float STEP = (float)(PI_DIV_2 / (double)MAX_INDEX);
        if(angle > PI_X2) {
            angle -= ((uint32_t)(angle / PI_X2)) * PI_X2;
        }
        if(angle <= PI_DIV_2) {
            const size_t index = (angle / STEP + 0.5);
            return sin_table[std::min(index,MAX_INDEX)];
        } else
        if(angle > PI_DIV_2 && angle <= PI) {
            const size_t index = ((PI - angle) / STEP + 0.5);
            return sin_table[std::min(index,MAX_INDEX)];
        } else
        if(angle > PI && angle <= PI_SUM_DIV_2) {
            const size_t index = ((angle - PI) / STEP + 0.5);
            return -sin_table[std::min(index,MAX_INDEX)];
        } else {
            const size_t index = ((PI_X2 - angle) / STEP + 0.5);
            return -sin_table[std::min(index,MAX_INDEX)];
        }
    }

    float get_cos(float angle) {
        const double PI = 3.1415926535897932384626433832795d;
        const double PI_DIV_2 = PI / 2.0d;
        const double PI_SUM_DIV_2 = PI + PI / 2.0d;
        const double PI_X2 = PI * 2.0d;
        const size_t MAX_INDEX = SIN_COS_TABLE_SIZE - 1;
        const float STEP = (float)(PI_DIV_2 / (double)MAX_INDEX);
        if(angle > PI_X2) {
            angle -= ((uint32_t)(angle / PI_X2)) * PI_X2;
        }
        if(angle <= PI_DIV_2) {
            const size_t index = (angle / STEP + 0.5);
            return cos_table[std::min(index, MAX_INDEX)];
        } else
        if(angle > PI_DIV_2 && angle <= PI) {
            const size_t index = ((PI - angle) / STEP + 0.5);
            return -cos_table[std::min(index, MAX_INDEX)];
        } else
        if(angle > PI && angle <= PI_SUM_DIV_2) {
            const size_t index = ((angle - PI) / STEP + 0.5);
            return -cos_table[std::min(index, MAX_INDEX)];
        } else {
            const size_t index = ((PI_X2 - angle) / STEP + 0.5);
            return cos_table[std::min(index, MAX_INDEX)];
        }
    }

    /** \brief Получить синус и косинус
     * \param cos_value Значение косинуса
     * \param sin_value Значение синуса
     * \param angle Угол от 0 до 2-х Пи
     */
    void get_sin_cos(float &cos_value, float &sin_value, const float angle) {
        const double PI = 3.1415926535897932384626433832795d;
        const double PI_DIV_2 = PI / 2.0d;
        const double PI_SUM_DIV_2 = PI + PI / 2.0d;
        const double PI_X2 = PI * 2.0d;
        const size_t MAX_INDEX = SIN_COS_TABLE_SIZE - 1;
        const float STEP = (float)(PI_DIV_2 / (double)MAX_INDEX);
        if(angle <= PI_DIV_2) {
            const size_t index = (angle / STEP + 0.5);
            cos_value = cos_table[std::min(index, MAX_INDEX)];
            sin_value = sin_table[std::min(index, MAX_INDEX)];
        } else
        if(angle > PI_DIV_2 && angle <= PI) {
            const size_t index = ((PI - angle) / STEP + 0.5);
            cos_value = -cos_table[std::min(index, MAX_INDEX)];
            sin_value = sin_table[std::min(index, MAX_INDEX)];
        } else
        if(angle > PI && angle <= PI_SUM_DIV_2) {
            const size_t index = ((angle - PI) / STEP + 0.5);
            cos_value = -cos_table[std::min(index, MAX_INDEX)];
            sin_value = -sin_table[std::min(index, MAX_INDEX)];
        } else {
            const size_t index = ((PI_X2 - angle) / STEP + 0.5);
            cos_value = cos_table[std::min(index, MAX_INDEX)];
            sin_value = -sin_table[std::min(index, MAX_INDEX)];
        }
    }
};

#endif // EASY_BO_MATH_HPP_INCLUDED
