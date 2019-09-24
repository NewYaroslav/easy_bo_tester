#ifndef EASY_BO_TESTER_COMMON_HPP_INCLUDED
#define EASY_BO_TESTER_COMMON_HPP_INCLUDED

#include "xtime.hpp"
#include <cmath>
#include <limits>

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

    /** \brief Бинарный опицон
     * Данный класс хранит данные о бинарном опционе
     */
    template<class T1>
    class BinaryOption {
    public:
        T1 payout = 0.0;                    /**< Выплата брокера. От 0.0, 1.0 соответствует 100% */
        xtime::timestamp_t timestamp = 0;   /**< Метка времени начала опицона */
        uint32_t duration = 0;              /**< Продолжительность опциона в секундах */
        int8_t direction = EASY_BO_NO_BET;  /**< Направление ставки, покупка или продажа опциона */
        int8_t result = EASY_BO_NEUTRAL;    /**< Результат опциона, победа или поражение */

        BinaryOption() {};

        BinaryOption(
                const T1 &p,
                const xtime::timestamp_t &t,
                const uint32_t &dur,
                const int8_t &dir,
                const int8_t &res) :
            payout(p),timestamp(t),duration(dur),direction(dir),result(res)  {
        };
    };

    /** \brief Посчитать математическое ожидание прибыли
     * Если winrate имеет недопустимое значение, функция вернет 0
     * \param winrate эффективность стратегии (от 0 до 1.0)
     * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
     * \param loss потери в случае поражения (обычно всегда 1.0)
     * \return математическое ожидание прибыли
     */
    template<class T>
    T calc_expectation(const T &winrate, const T &profit, const T &loss = 1.0) {
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
};

#endif // EASY_BO_TESTER_COMMON_HPP_INCLUDED
