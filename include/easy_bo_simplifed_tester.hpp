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
#ifndef EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED
#define EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED

#include "easy_bo_common.hpp"
#include <vector>

namespace easy_bo {

    /** \brief Класс простого тестера для измерения эфективности торговли бинарными опицонами
     */
    template<class UINT_TYPE = uint32_t>
    class SimplifedTester {
    private:
        UINT_TYPE wins = 0;  /**< Число удачных сделок */
        UINT_TYPE losses = 0;/**< Число убыточных сделок */
    public:
        SimplifedTester() {};

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
    };
};

#endif // EASY_BO_SIMPLIFED_TESTER_HPP_INCLUDED
