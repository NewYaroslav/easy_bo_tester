#ifndef EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED
#define EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED

#include "easy_bo_common.hpp"
#include <vector>

namespace easy_bo {

    /** \brief Класс простого тестера для измерения эфективности торговли бинарными опицонами
     */
    class SimplifedTester {
    private:
        uint32_t wins = 0;  /**< Число выигрышей */
        uint32_t losses = 0;/**< Число проигрышей */
    public:
        SimplifedTester() {};

        /** \brief Получить винрейт
         * \return Винрейт, число от 0.0 до 1.0
         */
        inline double get_winrate() {
            uint32_t sum = wins + losses;
            return sum == 0 ? 0.0 : (double)wins / (double)sum;
        }

        /** \brief Получить количество сделок
         * \return количество сделок
         */
        inline uint32_t get_deals() {
            return wins + losses;
        }

        /** \brief Получить число выигрышей
         * \return число выигрышей
         */
        inline uint32_t get_wins() {return wins;};

        /** \brief Получить число проигрышей
         * \return число проигрышей
         */
        inline uint32_t get_losses() {return losses;};

        /** \brief Добавить сделку
         * \param result Результат опциона
         */
        void add_deal(const int8_t &result) {
            if(result == EASY_BO_WIN) ++wins;
            else if(result == EASY_BO_LOSS) ++losses;
        }

        /** \brief Очистить состояние тестера
         * Данный метод обнулит все сделки
         */
        inline void clear() {
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
         * \param profit выплата брокера в случае успеха (обычно от 0 до 1.0, но можно больше 1.0)
         * \param loss потери в случае поражения (обычно всегда 1.0)
         * \return математическое ожидание прибыли
         */
        template<class T>
        T get_expected_value(const T &profit, const T &loss = 1.0) {
            return calc_expectation((T)get_winrate(), profit, loss);
        }
    };
};

#endif // EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED
