#ifndef EASY_BO_OPTIMIZATION_TESTER_HPP_INCLUDED
#define EASY_BO_OPTIMIZATION_TESTER_HPP_INCLUDED

#include "easy_bo_common.hpp"
#include <vector>

namespace easy_bo {

    /** \brief Класс тестера для оптимизаторов
     *
     * Данный класс приспособен считать бинарные опционы быстрее, чем StandardTester.
     * При этом данный класс не может учитывать время экспирации опциона.
     */
    template<class UINT_TYPE = uint32_t>
    class OptimizationTester {
    private:
        double start_deposit = 0;           /**< Начальный депозит */
        double gross_profit = 0;            /**< Общая прибыль (Gross Profit) — сумма всех прибыльных сделок в денежных единица */
        double gross_loss = 0;              /**< Общий убыток (Gross Loss) — сумма всех убыточных сделок в денежных единицах */
        UINT_TYPE wins = 0;                 /**< Число удачных сделок */
        UINT_TYPE losses = 0;               /**< Число убыточных сделок */
        std::vector<uint8_t> state;         /**< Массив состояний */
        std::vector<double> array_equity;   /**< Кривая средств. Это количество средств с учетом результатов по текущим открытым позициям */
    public:
        OptimizationTester(const size_t reserve_size = 100) {
            state.reserve(reserve_size);
            array_equity.reserve(reserve_size);
        };

        /** \brief Получить винрейт
         * \return Винрейт, число от 0.0 до 1.0
         */
        template<class FLOAT_TYPE>
        inline FLOAT_TYPE get_winrate() {
            UINT_TYPE sum = wins + losses;
            return sum == 0 ? 0.0 : (FLOAT_TYPE)wins / (FLOAT_TYPE)sum;
        }

        /** \brief Получить количество сделок
         * \return Количество сделок
         */
        inline UINT_TYPE get_deals() {
            return wins + losses;
        }

        /** \brief Получить количество выигрышей
         * \return Количество удачных сделок
         */
        inline UINT_TYPE get_wins() {return wins;};

        /** \brief Получить количество проигрышей
         * \return Количество убыточных сделок
         */
        inline UINT_TYPE get_losses() {return losses;};

        /** \brief Добавить сделку
         * \param result Результат опциона
         */
        template<class INT_TYPE>
        void add_deal(const INT_TYPE &result) {
            if(result == EASY_BO_WIN) state.push_back(1);
            else if(result == EASY_BO_LOSS) state.push_back(0);
        }

        /** \brief Остановить тестирование
         *
         * Данный метод подсчитает результаты бинарных опционов.
         * Необходимо вызывать данный метод по окончанию тестирования
         */
        void stop() {
            for(size_t i = 0; i < state.size(); ++i) {
                wins += state[i];
                losses += (1 - state[i]);
            }
        }

        /** \brief Очистить состояние тестера
         * Данный метод обнулит все сделки
         */
        inline void clear() {
            state.clear();
            array_equity.clear();
            wins = 0;
            losses = 0;
        }

        /** \brief Инициализировать тестер массивом сделок
         * \param array_deals массив сделок, например std::vector<BinaryOption>
         */
        template<class T>
        void init(const T &array_deals) {
            size_t array_size = array_deals.size();
            for(size_t i = 0; i < array_size; ++i) {
                add_deal(array_deals[i].result);
            }
        }

        /** \brief Инициализировать тестер массивом сделок
         * \param array_deals массив сделок, например std::vector<BinaryOption>
         */
        template<class T>
        SimplifedTester(const T &array_deals) {
            init(array_deals);
        };

        /** \brief Инициализировать тестер массивом сделок
         * \param array_deals массив результата сделок
         */
        void init(const std::vector<int8_t> &array_deals) {
            size_t array_size = array_deals.size();
            for(size_t i = 0; i < array_size; ++i) {
                add_deal(array_deals[i]);
            }
        }

        /** \brief Получить математическое ожидание прибыли
         *
         * Этот статистически рассчитываемый показатель отражает среднюю прибыльность/убыточность одной сделки
         * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
         * \param loss потери в случае поражения (обычно всегда 1.0)
         * \return Математическое ожидание прибыли, выраженное в процентах от ставки (например, 0.01 означает прибыль 1% от ставки).
         */
        template<class T>
        T get_expected_payoff(const T &profit, const T &loss = 1.0) {
            return calc_expected_payoff((T)get_winrate<T>(), profit, loss);
        }

        /** \brief Рассчитать кривую средств
         *
         * Данный метод нужен для рассчета кривой средств. Этот метод нужно вызывать перед получением коэффициента Шарпа, общей прибыли и пр.
         * \param Начальный депозит
         * \param broker_payout Выплата брокера
         * \param amount Размер ставки. Если меньше 1.0, то мы ставим процент от депозита, если 1.0 или больше - ставим абсолютное значение.
         */
        void calc_equity(const double bo_start_deposit, const double broker_payout, const double amount) {
            start_deposit = bo_start_deposit;
            array_equity.clear();
            array_equity.push_back(bo_start_deposit);
            if(amount > 1.0) {
                const double profit = broker_payout * amount;
                for(size_t i = 0; i < state.size(); ++i) {
                    if(array_equity.back() <= 0) break;
                    if(state[i]) {
                        array_equity.push_back(array_equity.back() + profit);
                        gross_profit += profit;
                    } else {
                        array_equity.push_back(array_equity.back() - amount);
                        gross_loss += amount;
                    }
                }
            } else {
                for(size_t i = 0; i < state.size(); ++i) {
                    if(array_equity.back() <= 0) break;
                    const double risk = amount * array_equity.back();
                    const double profit = broker_payout * risk;
                    if(state[i]) {
                        array_equity.push_back(array_equity.back() + profit);
                        gross_profit += profit;
                    } else {
                        array_equity.push_back(array_equity.back() - risk);
                        gross_loss += risk;
                    }
                }
            }
        }

        /** \brief Получить кривую средств
         *
         * Кривая средств, это количество средств с учетом результатов по текущим открытым позициям
         * \return кривая средств
         */
        std::vector<double> get_equity_curve() {return array_equity;};

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
         * \return Чистая прибыль (Total Net profit)
         */
        double get_total_net_profit() {return gross_profit - gross_loss;};

        /** \brief Получить прибыльность (Profit Factor)
         *
         * Прибыльность (Profit Factor) — отношение общей прибыли к общему убытку в процентах.
         * Единица означает, что сумма прибылей равна сумме убытков
         * \return Прибыльность (Profit Factor)
         */
        double get_profit_factor() {return gross_loss == 0.0 ? std::numeric_limits<float>::max() : gross_profit/gross_loss;};

        /** \brief Получить абсолютную просадку баланса (Balance Drawdown Absolute)
         *
         * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
         * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
         * Абсолютная просадка = Начальный депозит - Минимальный баланс.
         * \return Абсолютная просадка баланса (Balance Drawdown Absolute)
         */
        double get_balance_drawdown_absolute() {
            return calc_balance_drawdown_absolute<double>(array_equity);
        }

        /** \brief Получить максимальную просадку баланса (Balance Drawdown Maximal)
         *
         * Абсолютная просадка баланса (Balance Drawdown Absolute) — разница между значением начального депозита
         * и минимальным значением ниже начального депозита, до которого когда-либо падал баланс за всю историю счета.
         * Абсолютная просадка = Начальный депозит - Минимальный баланс.
         * \return Максимальная просадка баланса (Balance Drawdown Maximal)
         */
        double get_balance_drawdown_maximal() {
            return calc_balance_drawdown_maximal<double>(array_equity);
        }

        /** \brief Получить относительную просадку баланса (Balance Drawdown Relative)
         *
         * Относительная просадка баланса (Balance Drawdown Relative) — наибольшее падение баланса между локальным максимумом
         * и следующим локальным минимумом в процентах.
         * \return Относительная просадка баланса (Balance Drawdown Relative), значение от 0.0. до 1.0
         */
        double get_balance_drawdown_relative() {
            return calc_balance_drawdown_relative<double>(array_equity);
        }

        /** \brief Получить усиление депозита
         * \return усиление депозита
         */
        double get_gain() {
            if(array_equity.size() == 0) return 1.0;
            return array_equity.back() / start_deposit;
        }

        /** \brief Получить коэффициент Шарпа
         *
         * Данный вариант метода пригоден только для экспоненциального роста депозита
         * коэффициент Шарпа 1 и выше — оптимальное значение коэффициента,
         * обозначающее хорошую стратегию или высокую результативность управления портфелем ценных бумаг
         * \return Коэффициент Шарпа
         */
        double get_sharpe_ratio() {
            return calc_sharpe_ratio<double>(array_equity);
        }
    };
}

#endif // EASY_BO_OPTIMIZATION_TESTER_HPP_INCLUDED