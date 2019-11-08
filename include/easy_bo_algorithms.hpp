#ifndef EASY_BO_ALGORITHMS_HPP_INCLUDED
#define EASY_BO_ALGORITHMS_HPP_INCLUDED

#include "easy_bo_math.hpp"

namespace easy_bo {

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
        if(!easy_bo_math::is_generate_table) easy_bo_math::generate_table_sin_cos();
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
                easy_bo_math::get_sin_cos(mass_x, mass_y, angle - ((uint32_t)(angle / PI_X2)) * PI_X2);
            } else {
                easy_bo_math::get_sin_cos(mass_x, mass_y, angle);
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
        return 1.0/easy_bo_math::inv_sqrt(temp);
    }

    /** \brief Найти коэффициент лучшей стратегии
     *
     * Данная функция находит расстояне между лучшей точкой 3D пространства и точкой стратегии
     * Три оси 3D пространства соответствуют инверсному винрейту, стабильности стратегии (0 - лучшая стабильность), и инверсному количеству сделок
     * \param winrate Винрейт. Принимает значения от 0 до 1
     * \param stability Стабильность. Принимает значения от 0 до 1. Лучшее значение 0.
     * \param amount_deals Количество сделок
     * \param max_amount_deals Максимальное количество сделок
     * \param a1 Коэффициент корректировки
     * \param a2 Коэффициент корректировки
     * \param b1 Коэффициент корректировки
     * \param b2 Коэффициент корректировки
     * \param c1 Коэффициент корректировки
     * \param c2 Коэффициент корректировки
     * \return коэффициент лучшей стратегии
     */
    float calc_coeff_best3D(
            const float winrate,
            const float stability,
            const float amount_deals,
            const float max_amount_deals,
            const float a1 = 1.0,
            const float a2 = 1.0,
            const float b1 = 1.0,
            const float b2 = 0.0,
            const float c1 = 1.0,
            const float c2 = 1.0) {
        const float inv_winrate = a1 * (1.0 - winrate * a2);
        const float update_stability = stability * b1 + b2;
        const float relative_amount_deals = c1 * (1.0 - (amount_deals / max_amount_deals) * c2);
        const float temp = inv_winrate * inv_winrate + relative_amount_deals * relative_amount_deals + update_stability * update_stability;
        if(temp == 0.0) return 0.0;
        return 1.0/easy_bo_math::inv_sqrt(temp);
    }
}

#endif // EASY_BO_ALGORITHMS_HPP_INCLUDED
