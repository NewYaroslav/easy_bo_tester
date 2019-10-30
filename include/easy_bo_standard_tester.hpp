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
#ifndef EASY_BO_STANDARD_TESTER_HPP_INCLUDED
#define EASY_BO_STANDARD_TESTER_HPP_INCLUDED

#include "easy_bo_common.hpp"
#include "xtime.hpp"
#include <vector>
#include <algorithm>
#include <limits>

namespace easy_bo {

     /// Варианты рассчета просадки депозита
    enum {
        USE_BALANCE_CURVE = 0,  ///< Использовать кривую баланса
        USE_EQUITY_CURVE = 1,   ///< Использовать кривую средств
    };

    /** \brief Класс стандартного тестера для измерения эфективности торговли бинарными опицонами
     *
     * Данный класс позволяет получить винрейт, количество прибыльных и убыточных сделок,
     * кривую депозита и некоторые коэффициенты.
     */
    class StandardTester {
    private:
        xtime::timestamp_t start_timestamp = 0; /**< Начальная метка времени */
        uint64_t seconds_counter = 0;           /**< Счетчик секунд, нужен для определения момента завершения опицона */
        uint64_t wins = 0;                      /**< Число удачных сделок */
        uint64_t losses = 0;                    /**< Число убыточных сделок */
        double deposit;                         /**< Депозит для работы в режиме "реального времени" */
        double start_deposit;                   /**< Начальный депозит */
        double gross_profit = 0;                /**< Общая прибыль (Gross Profit) — сумма всех прибыльных сделок в денежных единица */
        double gross_loss = 0;                  /**< Общий убыток (Gross Loss) — сумма всех убыточных сделок в денежных единицах */

        /** \brief Класс для хранения депозита "в будущем"
         */
        class FutureDeposit {
        public:
            double increase = 0;    /**< Прирост депозита. Если прирост 0, значит сделка убточная */
            double amount = 0;      /**< Размер ставки. */
            uint64_t timestamp = 0; /**< Метка времени прироста депозита */

            FutureDeposit() {};

            FutureDeposit(const double &bo_increase, const double &bo_amount, const uint64_t &bo_timestamp) :
                increase(bo_increase), amount(bo_amount), timestamp(bo_timestamp) {};
        };

        std::vector<FutureDeposit> array_increase;                  /**< Массив прироста депозита */
        std::vector<double> array_balance;                          /**< Кривая баланса. Это количество средств на счете без учета результатов по текущим открытым позиция */
        std::vector<double> array_equity;                           /**< Кривая средств. Это количество средств с учетом результатов по текущим открытым позициям */
        std::vector<xtime::timestamp_t> array_balance_timestamp;    /**< Массив меток времени кривой баланса */
        std::vector<xtime::timestamp_t> array_equity_timestamp;     /**< Массив меток времени кривой средств */

        bool is_use_timestamp;  /**< Использовать метку времени или нет */

        void process(const uint64_t value) {
            /* если нет сделок - выходим */
            if(array_increase.size() == 0) return;
            /* если сделки есть, но даже самые ранние еще не закрылись - выходим*/
            if(array_increase[0].timestamp > value) return;
            bool is_update = false; // флаг обновления данных кривой средств и баланса
            uint64_t last_timestamp = array_increase[0].timestamp; // последняя метка времени в самом начале равна самой ранней метке времени
            double sum_increase = 0.0; // сумма приращений депозита
            uint32_t i;
            for(i = 0; i < array_increase.size(); ++i) {
                /* Проверяем, надо ли обновить кривую баланса и средств.
                 * Если очередной бинарный опицон имеем более позднее время закрытия,
                 * значит пора обновить кривую баланса и средств
                 */
                if(last_timestamp != array_increase[i].timestamp) {
                    /* обновляем кривую баланса */
                    array_balance.push_back(deposit);
                    array_balance_timestamp.push_back(last_timestamp);
                    /* обновляем кривую средств */
                    double temp = array_equity.back();
                    array_equity.push_back(temp + sum_increase);
                    array_equity_timestamp.push_back(last_timestamp);
                    /* */
                    sum_increase = 0.0;
                    last_timestamp = array_increase[i].timestamp;
                    is_update = false;
                }

                if(array_increase[i].timestamp > value) break;
                /* считаем статистику убыточных и доходных сделок,
                 * а также находим приращение депозита и пр. */
                if(array_increase[i].increase > 0.0) {
                    ++wins;
                    deposit += array_increase[i].increase;
                    deposit += array_increase[i].amount;
                    sum_increase += array_increase[i].increase;
                    gross_profit += array_increase[i].increase;
                }
                else {
                    ++losses;
                    sum_increase -= array_increase[i].amount;
                    gross_loss += array_increase[i].amount;
                }
                is_update = true;
            }
            if(is_update) {
                /* обновляем кривую баланса */
                array_balance.push_back(deposit);
                array_balance_timestamp.push_back(last_timestamp);
                /* обновляем кривую средств */
                double temp = array_equity.back();
                array_equity.push_back(temp + sum_increase);
                array_equity_timestamp.push_back(last_timestamp);
                /* */
            }
            /* удаляем из вектора завершенные сделки */
            array_increase.erase(array_increase.begin(), array_increase.begin() + i);
        }
    public:

        /** \brief Конструктор класса для использования тестера без меток времени
         *
         * Если использовать данный конструктор, то при совершении сделок указывать метку времени не нужно
         * \param bo_start_deposit Начальный депозит, по умолчанию 1000 у.е.
         */
        StandardTester(const double bo_start_deposit = 1000.0) :
            deposit(bo_start_deposit), start_deposit(bo_start_deposit), is_use_timestamp(false)  {
            /* добавляем начальный уровень депозита */
            array_balance.push_back(deposit);
            array_equity.push_back(deposit);
            array_balance_timestamp.push_back(0);
            array_equity_timestamp.push_back(0);
        };

        /** \brief Конструктор класса для использования тестера с меткой времени
         *
         * Если использовать данный конструктор, то при совершении сделок необходимо будет указывать метку времени
         * \param bo_start_deposit Начальный депозит
         * \param bo_start_timestamp Начальная метка времени
         */
        StandardTester(const double bo_start_deposit, const xtime::timestamp_t bo_start_timestamp) :
            deposit(bo_start_deposit), start_deposit(bo_start_deposit), is_use_timestamp(true)  {
            /* добавляем начальный уровень депозита */
            array_balance.push_back(deposit);
            array_equity.push_back(deposit);
            array_balance_timestamp.push_back(bo_start_timestamp);
            array_equity_timestamp.push_back(bo_start_timestamp);
            start_timestamp = bo_start_timestamp;
        };

        /** \brief Обновить задержку между сделками
         *
         * Данный метод необходимо переодически вызывать, например на таймфрейме 1 минута
         * этот метод можно вызывать в начале или конце каждой свечи или бара.
         * При этом задержку необходимо будет указать равной 60 секунд.
         * \param delay Задержка в секундах.
         */
        void update_delay(const uint64_t &delay) {
            /* увелчим таймер */
            seconds_counter += delay;
            process(seconds_counter);
        }

        /** \brief Обновить текущее время
         *
         * Данный метод необходимо переодически вызывать, например на таймфрейме 1 минута
         * этот метод можно вызывать в начале или конце каждой свечи или бара.
         * При этом необходимо указать текущую метку времени свечи или бара.
         * \param timestamp Метка времени.
         */
        void update_timestamp(const xtime::timestamp_t timestamp) {
            process(timestamp);
        }

        /** \brief Остановить тестирование
         *
         * Данный метод подсчитает результаты бинарных опционов, которые не были
         * обработаны методом update_delay или update_timestamp.
         * Рекомендуется вызывать данный метод по окончанию тестирования
         */
        void stop() {
            process(std::numeric_limits<uint64_t>::max());
        }

        /** \brief Добавить сделку с заранее известным результатом
         *
         * Данный метод позволяет  добавить бинарный опцион.
         * Сделка может учитывать как в конце теста, так во время теста, а взаивисмости от заданного режима.
         * \param result Результат опциона
         * \param duration Длительность экспирации опциона в секундах
         * \param broker_payout Выплата брокера
         * \param amount Размер ставки. Если меньше 1.0, то мы ставим процент от депозита, если 1.0 или больше - ставим абсолютное значение.
         * \param timestamp Метка времени бинарного опциона. По умолчанию 0. Если метка времени равна 0, значит алгоритм не использует метки времени
         * \param bo_direction Направление бинарного опциона.
         */
        void add_deal(  const int32_t result,
                        const uint32_t duration,
                        const double broker_payout,
                        const double amount,
                        const xtime::timestamp_t timestamp = 0) {
            if(deposit <= 0.0) return;
            /* находим время закрытия опциона и время открытия опицона */
            const uint64_t bo_closing_time = is_use_timestamp ? timestamp + duration : seconds_counter + duration;
            const uint64_t bo_opening_time = is_use_timestamp ? timestamp : seconds_counter;
            /* находим самый поздний опицон */
            auto array_increase_it = std::upper_bound(
                array_increase.begin(),
                array_increase.end(),
                bo_closing_time,
                [](const uint64_t &timestamp, const FutureDeposit &rhs) {
                return timestamp < rhs.timestamp;
            });

            /* проверяем, что мы используем: процент ставки или размер в абсолютном значении */
            if(amount >= 1.0) {
                deposit -= amount;
                /* добавляем прирост депозита */
                if(result == EASY_BO_WIN) array_increase.insert(array_increase_it, FutureDeposit(amount * broker_payout, amount, bo_closing_time));
                else array_increase.insert(array_increase_it, FutureDeposit(0, amount, bo_closing_time));
            } else {
                double temp = deposit * amount;
                deposit -= temp;
                /* добавляем прирост депозита */
                if(result == EASY_BO_WIN) array_increase.insert(array_increase_it, FutureDeposit(temp * broker_payout, temp, bo_closing_time));
                else array_increase.insert(array_increase_it, FutureDeposit(0, temp, bo_closing_time));
            }
            if(array_balance.size() == 0) {
                array_balance.push_back(deposit);
                array_balance_timestamp.push_back(bo_opening_time);
            } else {
                if(array_balance_timestamp.back() == bo_opening_time && array_balance.size() > 1) {
                    array_balance.back() = deposit;
                } else {
                    array_balance.push_back(deposit);
                    array_balance_timestamp.push_back(bo_opening_time);
                }
            }
        }

        /** \brief Очистить состояние тестера
         * Данный метод обнулит все сделки
         */
        inline void clear() {
            wins = 0;
            losses = 0;
            seconds_counter = 0;
            deposit =  start_deposit;
            gross_loss = 0;
            gross_profit = 0;
            array_balance.clear();
            array_equity.clear();
            array_balance_timestamp.clear();
            array_equity_timestamp.clear();
            array_increase.clear();
            if(is_use_timestamp) {
                array_balance.push_back(deposit);
                array_equity.push_back(deposit);
                array_balance_timestamp.push_back(start_timestamp);
                array_equity_timestamp.push_back(start_timestamp);
            } else {
                array_balance.push_back(deposit);
                array_equity.push_back(deposit);
                array_balance_timestamp.push_back(0);
                array_equity_timestamp.push_back(0);
            }
        }

        /** \brief Получить кривую баланса
         *
         * Кривая баланса, это количество средств на счете без учета результатов по текущим открытым позиция.
         * \return кривая баланса
         */
        std::vector<double> get_balance_curve() {return array_balance;};

        /** \brief Получить кривую средств
         *
         * Кривая средств, это количество средств с учетом результатов по текущим открытым позициям
         * \return кривая средств
         */
        std::vector<double> get_equity_curve() {return array_equity;};

        /** \brief Получить винрейт
         * \return Винрейт, число от 0.0 до 1.0, где 1.0 соответствует 100% винрейту
         */
        inline double get_winrate() {
            uint64_t sum = wins + losses;
            return sum == 0 ? 0.0 : (double)wins / (double)sum;
        }

        /** \brief Получить количество сделок
         * \return количество сделок
         */
        inline uint64_t get_deals() {
            return wins + losses;
        }

        /** \brief Получить число удачных сделок
         * \return Число удачных сделок
         */
        inline uint64_t get_wins() {return wins;};

        /** \brief Получить число убыточных сделок
         * \return Число убыточных сделок
         */
        inline uint64_t get_losses() {return losses;};

        /** \brief Получить математическое ожидание прибыли
         * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
         * \param loss потери в случае поражения (обычно всегда 1.0)
         * \return математическое ожидание прибыли
         */
        double get_expected_value(const double profit, const double loss = 1.0) {
            return calc_expectation(get_winrate(), profit, loss);
        }

        /** \brief Получить общую прибыль (Gross Profit)
         *
         * Общая прибыль (Gross Profit) — сумма всех прибыльных сделок в денежных единицах
         * \return Общая прибыль (Gross Profit)
         */
        double get_gross_profit() {return gross_profit;};

        /** \brief Получить общий убыток (Gross Loss)
         *
         * Общий убыток (Gross Loss) — сумма всех убыточных сделок в денежных единицах
         * \return Общий убыток (Gross Loss)
         */
        double get_gross_loss() {return gross_loss;};

        /** \brief Получить чистую прибыль (Total Net profit)
         *
         * Чистая прибыль (Total Net profit) — финансовый результат всех сделок.
         * Этот показатель представляет собой разность "Общей прибыли" и "Общего убытка"
         *
         * \return Чистая прибыль (Total Net profit)
         */
        double get_total_net_profit() {return gross_profit - gross_loss;};

        /** \brief Получить прибыльность (Profit Factor)
         *
         * Прибыльность (Profit Factor) — отношение общей прибыли к общему убытку в процентах.
         * Единица означает, что сумма прибылей равна сумме убытков
         * \return Прибыльность (Profit Factor)
         */
        double get_profit_factor() {return gross_loss == 0.0 ? 1.0 : gross_profit/gross_loss;};

        /** \brief Получить абсолютную просадку баланса (Balance Drawdown Absolute)
         *
         * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
         * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
         * Абсолютная просадка = Начальный депозит - Минимальный баланс.
         * \param curve_type Тип кривой. По умолчанию USE_EQUITY_CURVE - использовать кривую средств.
         * \return Абсолютная просадка баланса (Balance Drawdown Absolute)
         */
        double get_balance_drawdown_absolute(const uint8_t curve_type = USE_EQUITY_CURVE) {
            if(curve_type == USE_EQUITY_CURVE) return calc_balance_drawdown_absolute<double>(array_equity);
            else if(curve_type == USE_BALANCE_CURVE) return calc_balance_drawdown_absolute<double>(array_balance);
            return 0.0;
        }

        /** \brief Получить максимальную просадку баланса (Balance Drawdown Maximal)
         *
         * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
         * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
         * Абсолютная просадка = Начальный депозит - Минимальный баланс.
         * \param curve_type Тип кривой. По умолчанию USE_EQUITY_CURVE - использовать кривую средств.
         * \return Максимальная просадка баланса (Balance Drawdown Maximal)
         */
        double get_balance_drawdown_maximal(const uint8_t curve_type = USE_EQUITY_CURVE) {
            if(curve_type == USE_EQUITY_CURVE) return calc_balance_drawdown_maximal<double>(array_equity);
            else if(curve_type == USE_BALANCE_CURVE) return calc_balance_drawdown_maximal<double>(array_balance);
            return 0.0;
        }

        /** \brief Получить относительную просадку баланса (Balance Drawdown Relative)
         *
         * Относительная просадка баланса (Balance Drawdown Relative) — наибольшее падение баланса между локальным максимумом
         * и следующим локальным минимумом в процентах.
         * \param curve_type Тип кривой. По умолчанию USE_EQUITY_CURVE - использовать кривую средств.
         * \return Относительная просадка баланса (Balance Drawdown Relative), значение от 0.0. до 1.0
         */
        double get_balance_drawdown_relative(const uint8_t curve_type = USE_EQUITY_CURVE) {
            if(curve_type == USE_EQUITY_CURVE) return calc_balance_drawdown_relative<double>(array_equity);
            else if(curve_type == USE_BALANCE_CURVE) return calc_balance_drawdown_relative<double>(array_balance);
            return 0.0;
        }

        /** \brief Получить коэффициент Шарпа
         *
         * Данный вариант функции пригоден для экспоненциального роста депозита
         * коэффициент Шарпа 1 и выше — оптимальное значение коэффициента,
         * обозначающее хорошую стратегию или высокую результативность управления портфелем ценных бумаг
         * \param curve_type Тип кривой. По умолчанию USE_EQUITY_CURVE - использовать кривую средств.
         * \return Коэффициент Шарпа
         */
        double get_sharpe_ratio(const uint8_t curve_type = USE_EQUITY_CURVE) {
            if(curve_type == USE_EQUITY_CURVE) return calc_sharpe_ratio<double>(array_equity);
            else if(curve_type == USE_BALANCE_CURVE) return calc_sharpe_ratio<double>(array_balance);
            return 0.0;
        }
    };
}

#endif // EASY_BO_STANDARD_TESTER_HPP_INCLUDED
