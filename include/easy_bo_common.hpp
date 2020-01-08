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
#ifndef EASY_BO_TESTER_COMMON_HPP_INCLUDED
#define EASY_BO_TESTER_COMMON_HPP_INCLUDED

#include "xtime.hpp"
#include <cmath>
#include <limits>
#include <algorithm>
#include "easy_bo_math.hpp"

namespace easy_bo {

    /// Набор возможных состояний и настроек
    enum {
        EASY_BO_BUY = 1,    ///< Бинарный опицон на покупку
        EASY_BO_SELL = -1,  ///< Бинарный опицон на продажу
        EASY_BO_NO_BET = 0, ///< Нет ставки\бинарного опциона
        EASY_BO_WIN = 1,    ///< Успешный бинарный опицон
        EASY_BO_LOSS = -1,  ///< Убычтоный бинарный опцион
        EASY_BO_NEUTRAL = 0,///< Бинарный опцон в ничью
    };

	/// Набор возможных состояний ошибки
	enum {
		OK = 0,                 ///< Ошибок нет, все в порядке
		NO_DATA_ACCESS = -1,    ///< Нет доступа к данным
		UNKNOWN_ERROR = -3,     ///< Неопределенная ошибка
		PARSER_ERROR = -4,      ///< Ошибка парсера
		INVALID_PARAMETER = -6, ///< Один из параметров неверно указан
	};

    /** \brief Класс бинарного опицона
     *
     * Класс бинарного опциона нужен для хранения праметров бинарного опциона
     */
    template<class FLOAT_TYPE>
    class BinaryOption {
    public:
        FLOAT_TYPE payout = 0.0;            /**< Выплата брокера. Значение 1.0 соответствует 100% */
        xtime::timestamp_t timestamp = 0;   /**< Метка времени начала бинарного опицона */
        uint32_t duration = 0;              /**< Продолжительность бинарного опциона в секундах */
        int8_t direction = EASY_BO_NO_BET;  /**< Направление ставки, покупка или продажа опциона */
        int8_t result = EASY_BO_NEUTRAL;    /**< Результат опциона, победа или поражение */

        BinaryOption() {};

        BinaryOption(
                const int8_t &bo_result,
                const uint32_t &bo_duration,
                const FLOAT_TYPE &bo_payout,
                const xtime::timestamp_t &bo_timestamp,
                const int8_t &bo_direction) :
            payout(bo_payout), timestamp(bo_timestamp), duration(bo_duration), direction(bo_direction), result(bo_result)  {
        };
    };

    /** \brief Посчитать математическое ожидание прибыли
     *
     * Если winrate имеет недопустимое значение, функция вернет 0
     * \param winrate эффективность стратегии (от 0 до 1.0)
     * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
     * \param loss потери в случае поражения (обычно всегда 1.0)
     * \return Математическое ожидание прибыли. Значение в процентах от ставки (например, 0.01 означает прибыль 1% от ставки)
     */
    template<class T>
    T calc_expected_payoff(const T &winrate, const T &profit, const T &loss = 1.0) {
        if(winrate > 1.0 || winrate < 0.0) return 0.0;
        return (winrate * profit) - ((1.0 - winrate) * loss);
    }

    /** \brief Посчитать минимальную эффективность стратегии при заданном уровне выплат
     * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
     * \param loss потери в случае поражения (обычно всегда 1.0)
     * \return минимальная эффективность стратегии
     */
    template<class T>
    T calc_min_winrate(const T &profit, const T &loss = 1.0) {
            return loss / (profit + loss);
    }

    /** \brief Посчитать оптимальный процент ставки от депозита по критерию Келли
     *
     * Если прибыль невозможна из-за низкого математического ожидания, вернет 0
     * \param winrate эффективность стратегии (от 0 до 1.0)
     * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
     * \param attenuation коэффициент ослабления (рекомендуемое значение по умолчанию 0.4)
     * \return процент ставки (от 0 до 1.0)
     */
    template<class T>
    T calc_kelly_bet(const T &winrate, const T &profit, const T &attenuation = 0.4) {
        if(winrate <= calc_min_winrate(profit)) return 0.0;
        return attenuation * (((profit + 1.0) * winrate - 1.0) / profit);
    }

    /** \brief Посчитать стабильность прибыли
     *
     * Данный параметр тем лучше, чем кривая депозита ближе к экспоненте (для стратегии ставок по критерию Келли)
     * Первый элемент массива должен быть начальным уровнем депозита (депозит до первой сделки)
     * \param array_depo массив депозита
     * \return значение консистенции
     */
    template<class T1, class T2>
    T1 calc_profit_stability(const T2 &array_depo) {
        size_t size = array_depo.size();
        if(size == 0) return  0.0;
        T1 start_depo = std::log(array_depo.front());
        T1 stop_depo = std::log(array_depo.back());
        T1 delta = (T1)(stop_depo - start_depo) / (T1)(size - 1);
        T1 sum = 0;
        for(size_t i = 1; i < array_depo.size(); ++i) {
            T1 y = start_depo + delta * (T1)i;
            T1 diff = std::log(array_depo[i]) - y;
            sum += diff * diff;
        }
        sum /= (T1)(size - 1);
        return sum;
    }

    /** \brief Посчитать среднюю геометрическую доходность
     *
     * Первый элемент массива должен быть начальным уровнем депозита (депозит до первой сделки)
     * Данный вариант функции пригоден для экспоненциального роста депозита
     * \param array_depo массив депозита
     * \return средняя геометрическая доходность
     */
    template<class T1, class T2>
    T1 calc_geometric_average_return(const T2 &array_depo) {
        size_t size = array_depo.size();
        if(size < 1) return 0.0;
        T1 mx = 1.0;
        for(size_t i = 1; i < size; i++) {
            T1 ri = array_depo[i - 1] > 0.0 ?
                1.0 + ((T1)(array_depo[i] - array_depo[i - 1]) / (T1)array_depo[i - 1]) : 0;
            mx *= ri;
        }
        return std::pow(mx, 1.0/(T1)(size - 1)) - 1.0;
    }

    /** \brief Посчитать коэффициент Шарпа
     *
     * Первый элемент массива должен быть начальным уровнем депозита (депозит до первой сделки)
     * Данный вариант функции пригоден для экспоненциального роста депозита
     * коэффициент Шарпа 1 и выше — оптимальное значение коэффициента,
     * обозначающее хорошую стратегию или высокую результативность управления портфелем ценных бумаг
     * \param array_depo массив депозита
     * \return коэффициент Шарпа
     */
    template<class T1, class T2>
    T1 calc_sharpe_ratio(const T2 &array_depo) {
        T1 re = calc_geometric_average_return<T1>(array_depo);
        if(re == 0) return 0.0;
        T1 sum = 0;
        size_t size = array_depo.size();
        for(size_t i = 1; i < size; ++i) {
            T1 ri = array_depo[i - 1] > 0 ?
                ((T1)(array_depo[i] - array_depo[i - 1]) / (T1)array_depo[i - 1]) : 0;
            T1 diff = ri - re;
            sum += diff * diff;
        }

        if(sum == 0 && re > 0) return std::numeric_limits<T1>::max();
        else if(sum == 0 && re < 0) return std::numeric_limits<T1>::lowest();
        // так как в вычислениях первый элемент - начальный уровень депозита, то size - 2
        sum /= (T1)(size - 2);
        return (re / std::sqrt(sum));
    }

    /** \brief Посчитать коэффициент Шарпа быстро
     *
     * Первый элемент массива должен быть начальным уровнем депозита (депозит до первой сделки)
     * Данный вариант функции пригоден для экспоненциального роста депозита
     * коэффициент Шарпа 1 и выше — оптимальное значение коэффициента,
     * обозначающее хорошую стратегию или высокую результативность управления портфелем ценных бумаг
     * \param array_depo массив депозита
     * \return коэффициент Шарпа
     */
    template<class T1, class T2>
    T1 calc_fast_sharpe_ratio(const T2 &array_depo) {
        T1 re = calc_geometric_average_return<T1>(array_depo);
        if(re == 0) return 0.0;
        T1 sum = 0;
        size_t size = array_depo.size();
        for(size_t i = 1; i < size; ++i) {
            T1 ri = array_depo[i - 1] > 0 ?
                ((T1)(array_depo[i] - array_depo[i - 1]) / (T1)array_depo[i - 1]) : 0;
            T1 diff = ri - re;
            sum += diff * diff;
        }

        if(sum == 0 && re > 0) return std::numeric_limits<T1>::max();
        else if(sum == 0 && re < 0) return std::numeric_limits<T1>::lowest();
        // так как в вычислениях первый элемент - начальный уровень депозита, то size - 2
        sum /= (T1)(size - 2);
        return (re * easy_bo_math::inv_sqrt(sum));
    }

    /** \brief Рассчитать абсолютную просадку баланса (Balance Drawdown Absolute)
     *
     * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
     * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
     * Абсолютная просадка = Начальный депозит - Минимальный баланс.
     * \attention Первый элемент массива curve должен быть равень начальному значению депозита.
     * \attention Функция вернет положительное значение, если просадка есть, или 0, если просадки нет.
     * \param curve Массив значений кривой баланса
     * \return Абсолютная просадка баланса (Balance Drawdown Absolute)
     */
    template<class T1, class T2>
    T1 calc_balance_drawdown_absolute(const T2 &curve) {
        if(curve.size() == 0) return 0.0;
        auto curve_min_it = std::min_element(curve.begin(), curve.end());
        if(curve_min_it == curve.end() || *curve_min_it == curve[0]) return 0.0;
        return curve[0] - *curve_min_it;
    }

    /** \brief Рассчитать максимальную просадку баланса (Balance Drawdown Maximal)
     *
     * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
     * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
     * Абсолютная просадка = Начальный депозит - Минимальный баланс.
     * \attention Первый элемент массива curve должен быть равень начальному значению депозита.
     * \attention Функция вернет положительное значение, если просадка есть, или 0, если просадки нет.
     * \param curve Массив значений кривой баланса
     * \return Максимальная просадка баланса (Balance Drawdown Maximal)
     */
    template<class T1, class T2>
    T1 calc_balance_drawdown_maximal(const T2 &curve) {
        if(curve.size() == 0) return 0.0;
        using NUM_TYPE = typename T2::value_type;
        NUM_TYPE max_value = curve[0];
        NUM_TYPE max_difference = 0;
        for(size_t i = 1; i < curve.size(); ++i) {
            if(curve[i] > max_value) max_value = curve[i];
            if(curve[i] < max_value) {
                NUM_TYPE difference = max_value - curve[i];
                if(difference > max_difference) max_difference = difference;
            }
        }
        return (T1)max_difference;
    }

    /** \brief Рассчитать относительную просадку баланса (Balance Drawdown Relative)
     *
     * Относительная просадка баланса (Balance Drawdown Relative) — наибольшее падение баланса между локальным максимумом
     * и следующим локальным минимумом в процентах.
     * \attention Первый элемент массива curve должен быть равень начальному значению депозита.
     * \attention Функция вернет положительное значение от 0 до 1, если просадка есть, или 0, если просадки нет.
     * \param curve Массив значений кривой баланса
     * \return Относительная просадка баланса (Balance Drawdown Relative), значение от 0.0. до 1.0
     */
    template<class T1, class T2>
    T1 calc_balance_drawdown_relative(const T2 &curve) {
        if(curve.size() == 0) return 0.0;
        using NUM_TYPE = typename T2::value_type;
        NUM_TYPE max_value = curve[0];
        NUM_TYPE max_difference = 0;
        for(size_t i = 1; i < curve.size(); ++i) {
            if(curve[i] > max_value) max_value = curve[i];
            if(curve[i] < max_value) {
                NUM_TYPE difference = max_value - curve[i];
                difference /= (NUM_TYPE)max_value;
                if(difference > max_difference) max_difference = difference;
            }
        }
        return (T1)max_difference;
    }
};

#endif // EASY_BO_TESTER_COMMON_HPP_INCLUDED
