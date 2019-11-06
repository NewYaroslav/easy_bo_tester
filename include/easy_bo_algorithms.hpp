#ifndef EASY_BO_ALGORITHMS_HPP_INCLUDED
#define EASY_BO_ALGORITHMS_HPP_INCLUDED

#include <cmath>

namespace easy_bo {

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

    /** \brief Рассчитать центройд окружности
     *
     * Данная функция принимает на вход массив исхода бинарных опционов, где 1 - удачная сделка, 0 - убыточная сделка.
     * Или можно подать на вход профит. Функция найдет модуль вектора между центром окружности и центром ее тяжести.
     * Самое лучшее значение данного коэффициента - 0. Самое худшее значение - 1.0
     * Функция также имеет настройки через шаблоны.
     * is_binary - Флаг указывает, что мы используем бинарные данные.
     * is_use_negative - Флаг указывает, что мы используем убыточные сделки как отрицательную массу.
     * \param array_profit Массив результата сделок
     * \param array_profit_size Размер массива результата сделок
     * \param revolutions Количество оборотов окружности, по умолчанию 1. Можно взять 3,5,7 оборота, чтобы исключить влияние  повторяющихся неоднородностей
     * \return модуль вектора между центром окружности и центром ее тяжести
     */
    template<const bool is_binary, const bool is_use_negative, class ARRAY_TYPE>
    float calc_centroid_circle(const ARRAY_TYPE &array_profit, const size_t array_profit_size, const uint32_t revolutions = 1) {
        if(!is_generate_table) generate_table_sin_cos();
        const double PI = 3.1415926535897932384626433832795;
        const double PI_X2 = PI * 2.0;
        float angle = 0.0;
        float step = (float)((PI_X2 * (double)revolutions) / (double)array_profit_size);
        float sum_x = 0.0, sum_y = 0.0;
        float center_x = 0.0, center_y = 0.0;
        uint32_t counter = 0;
        for(size_t i = 0; i < array_profit_size; ++i, angle += step) {
            /* если "вес" элемента массива нулевой, нет смысла обрабатывать его */
            if(!is_use_negative) {
                if(array_profit[i] == 0) continue;
                ++counter;
            }
            float mass_x, mass_y;
            if(angle > PI_X2) {
                get_sin_cos(mass_x, mass_y, angle - ((uint32_t)(angle / PI_X2)) * PI_X2);
            } else {
                get_sin_cos(mass_x, mass_y, angle);
            }
            if(is_binary) {
                /* если "вес" элемента массива нулевой, вычитаем */
                if(array_profit[i] == 0) {
                    sum_x -= mass_x;
                    sum_y -= mass_y;
                } else {
                    sum_x += mass_x;
                    sum_y += mass_y;
                }
            } else {
                sum_x += array_profit[i] * mass_x;
                sum_y += array_profit[i] * mass_y;
            }
        }
        if(is_use_negative) counter = array_profit_size;
        center_x = sum_x / (float)counter;
        center_y = sum_y / (float)counter;
        float temp = center_x * center_x + center_y * center_y;
        if(temp == 0.0) return 0.0;
        return 1.0/inv_sqrt(temp);
    }

    /** \brief Найти коэффициент лучшей стратегии
     *
     * Данная функция находит расстояне между лучшей точкой 3D пространства и точкой стратегии
     * Три оси 3D пространства соответствуют инверсному винрейту, стабильности стратегии (0 - лучшая стабильность), и инверсному количеству сделок
     * \param winrate Винрейт. Принимает значения от 0 до 1
     * \param stability Стабильность. Принимает значения от 0 до 1. Лучшее значение 0.
     * \param amount_deals Количество сделок
     * \param max_amount_deals Максимальное количество сделок
     * \param a Коэффициент корректировки
     * \param b Коэффициент корректировки
     * \param c Коэффициент корректировки
     * \return коэффициент лучшей стратегии
     */
    float calc_coeff_best3D(
            const float winrate,
            const float stability,
            const float amount_deals,
            const float max_amount_deals,
            const float a = 1.0,
            const float b = 1.0,
            const float c = 1.0) {
        const float inv_winrate = a * (1.0 - winrate);
        const float relative_amount_deals = b * (1.0 - (amount_deals / max_amount_deals));
        const float update_stability = stability * c;
        const float temp = inv_winrate * inv_winrate + relative_amount_deals * relative_amount_deals + update_stability * update_stability;
        if(temp == 0.0) return 0.0;
        return 1.0/inv_sqrt(temp);
    }
}

#endif // EASY_BO_ALGORITHMS_HPP_INCLUDED
