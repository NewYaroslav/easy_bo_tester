#include <iostream>
#include "easy_bo_standard_tester.hpp"
#include <random>
#include <ctime>

int main() {
    std::cout << "standart tester" << std::endl;
    std::cout << "start test-1" << std::endl;
    easy_bo::StandardTester iStandardTester;
    /* совершаем ставки размером 10, с экспирацией 180
     * Т.к. выплата составляет 100%, в конец теста мы не должны ничего потерять,
     * депозит в начале теста будет равен депозиту в конце теста
     */
    for(uint32_t i = 0; i < 20; ++i) {
        iStandardTester.add_deal(easy_bo::EASY_BO_WIN, 180, 1.0, 10);
        iStandardTester.add_deal(easy_bo::EASY_BO_LOSS, 180, 1.0, 10);
        iStandardTester.update_delay(60);
    }
    iStandardTester.stop();
    std::cout << "test-1 winrate: " << iStandardTester.get_winrate() << std::endl;
    std::cout << "test-1 wins: " << iStandardTester.get_wins() << std::endl;
    std::cout << "test-1 losses: " << iStandardTester.get_losses() << std::endl;
    std::cout << "test-1 gain: " << iStandardTester.get_gain() << std::endl;
    std::vector<double> balance = iStandardTester.get_balance_curve();
    std::vector<double> equity = iStandardTester.get_equity_curve();
    for(size_t i = 0; i < balance.size(); ++i) {
        std::cout << "test-1 balance[" << i << "]: " << balance[i] << std::endl;
    }
    for(size_t i = 0; i < equity.size(); ++i) {
        std::cout << "test-1 equity[" << i << "]: " << equity[i] << std::endl;
    }

    /* Теперь проверим тест с вероятность успешной сделки 56%
     * выплатой 80% и ставкой 3%
     */
    std::cout << "start test-2" << std::endl;
    /* инициализируем генератор случайных чисел */
    std::mt19937 gen;
    gen.seed(time(0));
    std::uniform_int_distribution<> denominator(0, 1000);
    /* очистим тестер */
    iStandardTester.clear();
    /* начинаем имитацию торговли */
    for(uint32_t i = 0; i < 1000; ++i) {
        double p = (double)denominator(gen)/1000.0;
        if(p <= 0.56) iStandardTester.add_deal(easy_bo::EASY_BO_WIN, 180, 0.8, 0.03);
        else iStandardTester.add_deal(easy_bo::EASY_BO_LOSS, 180, 0.8, 0.03);
        iStandardTester.update_delay(60);
    }
    /* завершаем торговлю */
    iStandardTester.stop();
    /* выводим некоторые статистические данные */
    std::cout << "test-2 winrate: " << iStandardTester.get_winrate() << std::endl;
    std::cout << "test-2 wins: " << iStandardTester.get_wins() << std::endl;
    std::cout << "test-2 losses: " << iStandardTester.get_losses() << std::endl;
    std::cout << "test-2 gain: " << iStandardTester.get_gain() << std::endl;
    std::cout << "test-2 profit factor: " << iStandardTester.get_profit_factor() << std::endl;
    std::cout << "test-2 balance drawdown relative: " << iStandardTester.get_balance_drawdown_relative() << std::endl;
    std::cout << "test-2 sharpe ratio: " << iStandardTester.get_sharpe_ratio() << std::endl;
    std::cout << "test-2 fast sharpe ratio: " << iStandardTester.get_fast_sharpe_ratio() << std::endl;
    std::cout << "test-2 expected payoff: " << iStandardTester.get_expected_payoff(0.8) << std::endl;
    std::cout << "test-2 total net profit: " << iStandardTester.get_total_net_profit() << std::endl;
    return 0;
}
