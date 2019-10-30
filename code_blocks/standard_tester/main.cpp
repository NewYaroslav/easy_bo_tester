#include <iostream>
#include "easy_bo_standard_tester.hpp"
#include <random>
#include <ctime>

int main() {
    std::cout << "standart tester" << std::endl;
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
    std::cout << "winrate: " << iStandardTester.get_winrate() << std::endl;
    std::cout << "wins: " << iStandardTester.get_wins() << std::endl;
    std::cout << "losses: " << iStandardTester.get_losses() << std::endl;
    std::vector<double> balance = iStandardTester.get_balance_curve();
    std::vector<double> equity = iStandardTester.get_equity_curve();
    for(size_t i = 0; i < balance.size(); ++i) {
        std::cout << "balance[" << i << "]: " << balance[i] << std::endl;
    }
    for(size_t i = 0; i < equity.size(); ++i) {
        std::cout << "equity[" << i << "]: " << equity[i] << std::endl;
    }
    /* Теперь проверим тест с вероятность успешной сделки 56%
     * выплатой 80% и ставкой 1%
     */
    std::mt19937 gen;
    gen.seed(time(0));
    std::uniform_int_distribution<> denominator(0, 1000);
    iStandardTester.clear();
    for(uint32_t i = 0; i < 1000; ++i) {
        double p = (double)denominator(gen)/1000.0;
        if(p <= 0.56) iStandardTester.add_deal(easy_bo::EASY_BO_WIN, 180, 0.8, 0.01);
        else iStandardTester.add_deal(easy_bo::EASY_BO_LOSS, 180, 0.8, 0.01);
        iStandardTester.update_delay(60);
    }
    iStandardTester.stop();
    std::cout << "test-2 winrate: " << iStandardTester.get_winrate() << std::endl;
    std::cout << "test-2 wins: " << iStandardTester.get_wins() << std::endl;
    std::cout << "test-2 losses: " << iStandardTester.get_losses() << std::endl;
    std::cout << "test-2 profit factor: " << iStandardTester.get_profit_factor() << std::endl;
    std::cout << "test-2 balance drawdown relative: " << iStandardTester.get_balance_drawdown_relative() << std::endl;
    std::cout << "test-2 sharpe ratio: " << iStandardTester.get_sharpe_ratio() << std::endl;
    return 0;
}
