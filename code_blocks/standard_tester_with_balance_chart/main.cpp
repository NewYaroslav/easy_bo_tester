#include <iostream>
#include "easy_bo_standard_tester.hpp"
#include "easy_plot.hpp"
#include "xtechnical_indicators.hpp"
#include "xquotes_history.hpp"
#include <random>
#include <ctime>

int main(int argc, char* argv[]) {
    /* инициализируем библиотеку для рисования графиков */
    easy_plot::init(&argc, argv);

    /* инициализируем хранилище котировок */
    std::string path = "..//..//..//fxcm_history_quotes//storage//";
    std::string symbol_path = path + "AUDCAD.qhs4";
    xquotes_history::QuotesHistory<> iQuotesHistory(symbol_path, xquotes_common::PRICE_OHLC, xquotes_common::USE_COMPRESSION);

    /* инициализируем Боллинджер с периодом 20 и множителем 2 */
    xtechnical_indicators::BollingerBands<double> iBB(20,2);

    /* найдем минимальную и максимальную даты (время GMT)
     * начнем торговать с 2012 года
     */
    xtime::timestamp_t timestamp_min, timestamp_max;
    iQuotesHistory.get_min_max_day_timestamp(timestamp_min, timestamp_max);
    timestamp_min = std::max(timestamp_min, xtime::get_timestamp(1,1,2013));
    std::cout << "date: " << xtime::get_str_date(timestamp_min) << " - " << xtime::get_str_date(timestamp_max) << std::endl;

    /* Создадим тестер с начальным депозитом 100000, а также будем использовать метки времени */
    const double start_deposit = 100000.0;
    easy_bo::StandardTester iStandardTester(start_deposit, timestamp_min);

    /* настраиваем параметры торговли */
    const uint32_t duration = 180; // Время экспирации опицона 3 минуты
    const double broker_payout = 0.8; // Выплата брокера
    const double risk = 0.01; // Ставка в процентах от депозита, ставим 1%

    /* настраиваем параметры графика */
    easy_plot::WindowSpec wstyle;
    wstyle.grid_period = 0.1;

    /* начинаем торговать на минутном таймфрейме */
    for(xtime::timestamp_t t = timestamp_min; t <= timestamp_max; t += xtime::SECONDS_IN_MINUTE) {
        /* Обновим время тестера */
        iStandardTester.update_timestamp(t);

        xquotes_common::Candle candle; // свеча
        /* получаем минутную свечу */
        int err_candle = iQuotesHistory.get_candle(candle, t);
        if(err_candle != xquotes_common::OK) {
            /* если данные по свече отсутствуют,
             * очистим состояние индикатора,
             * чтобы начать обрабатывать котировки заново
             */
            iBB.clear();
            continue;
        }

        /* получаем полосы Боллинджера и его среднюю линию, используя цену закрытия свечи */
        double tl, ml, bl;
        int err_bb = iBB.update(candle.close, tl, ml, bl);
        if(err_bb != xtechnical_common::OK) continue; // если индикатор еще не готов нам дать результат, пропускаем ход

        /* Фильтруем по времени, мы будем торговать только с 0:00 до 3:00 часов по МСК
         * Так как у нас время в GMT, не забываем отнять 3 часа.
         * В это время мы будем торговать, так как почему то все брокеры делают "технический перерыв" в это время
         * Вдруг брокеры нам мешают заработать свои миллионы? Ща выясним!
         */
        if(xtime::get_hour_day(t) < 21) continue;

        /* Проверяем сигнал простой ТС,
         * если цена закрытяи свечи выша за линию, открываем сделку в противоположную сторону
         */
        int state_bo = xquotes_common::NEUTRAL; // состояние сделки, может иметь три состояния (WIN, LOSS, NEUTRAL)
        if(candle.close > tl) {
            /* получаем результат бинарного опциона. С этим надо быть поаккуратнее, т.к. можно "подсмотреть в будущее" */
            int err_bo = iQuotesHistory.check_binary_option(state_bo, xquotes_common::SELL, duration, t);
            if(err_bo != xquotes_common::OK) continue; // если данные по бинарному опциону не доступны, пропусакем ход

            /* добавляем сделку в тестер */
            iStandardTester.add_deal(state_bo, duration,  broker_payout, risk);
        }
        if(candle.close < bl) {
            /* получаем результат бинарного опциона. С этим надо быть поаккуратнее, т.к. можно "подсмотреть в будущее" */
            int err_bo = iQuotesHistory.check_binary_option(state_bo, xquotes_common::BUY, duration, t);
            if(err_bo != xquotes_common::OK) continue; // если данные по бинарному опциону не доступны, пропусакем ход

            /* добавляем сделку в тестер */
            iStandardTester.add_deal(state_bo, duration,  broker_payout, risk);
        }
        /* выводим дату */
        std::cout << std::endl << "date: " << xtime::get_str_date(t) << std::endl;

        /* выводим некоторые статистические данные */
        std::cout << "winrate: " << iStandardTester.get_winrate() << std::endl;
        std::cout << "wins: " << iStandardTester.get_wins() << std::endl;
        std::cout << "losses: " << iStandardTester.get_losses() << std::endl;
        std::cout << "profit factor: " << iStandardTester.get_profit_factor() << std::endl;
        std::cout << "balance drawdown relative: " << iStandardTester.get_balance_drawdown_relative() << std::endl;
        std::cout << "sharpe ratio: " << iStandardTester.get_sharpe_ratio() << std::endl;
        std::cout << "expected payoff: " << iStandardTester.get_expected_payoff(0.8) << std::endl;
        std::cout << "total net profit: " << iStandardTester.get_total_net_profit() << std::endl;

        /* рисуем график средств на счете */
        std::vector<double> equity_curve =  iStandardTester.get_equity_curve();
        easy_plot::plot<double>("equity", wstyle, (int)1, equity_curve, easy_plot::LineSpec(0.1, 0.5, 1.0));
    }

    /* останавливаем тестирование */
    iStandardTester.stop();
    std::cout << std::endl << std::endl;
    /* выводим некоторые статистические данные */
    std::cout << "winrate: " << iStandardTester.get_winrate() << std::endl;
    std::cout << "wins: " << iStandardTester.get_wins() << std::endl;
    std::cout << "losses: " << iStandardTester.get_losses() << std::endl;
    std::cout << "profit factor: " << iStandardTester.get_profit_factor() << std::endl;
    std::cout << "balance drawdown relative: " << iStandardTester.get_balance_drawdown_relative() << std::endl;
    std::cout << "sharpe ratio: " << iStandardTester.get_sharpe_ratio() << std::endl;
    std::cout << "expected payoff: " << iStandardTester.get_expected_payoff(0.8) << std::endl;
    std::cout << "total net profit: " << iStandardTester.get_total_net_profit() << std::endl;

    /* рисуем график средств на счете */
    std::vector<double> equity_curve =  iStandardTester.get_equity_curve();
    easy_plot::plot<double>("equity", wstyle, (int)1, equity_curve, easy_plot::LineSpec(0.1, 0.5, 1.0));
    std::cout << "end of test, date: " << xtime::get_str_date(timestamp_min) << " - " << xtime::get_str_date(timestamp_max) << std::endl;

    std::system("pause");
    return 0;
}
