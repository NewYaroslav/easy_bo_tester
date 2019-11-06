#include <iostream>
#include "easy_bo_optimization_tester.hpp"
#include <random>
#include <ctime>

int main() {
    std::cout << "standart tester" << std::endl;
    std::cout << "start test-1" << std::endl;
    easy_bo::OptimizationTester<uint32_t> iOptimizationTester;
    /* совершаем ставки размером 10, с экспирацией 180
     * Т.к. выплата составляет 100%, в конец теста мы не должны ничего потерять,
     * депозит в начале теста будет равен депозиту в конце теста
     */
    for(uint32_t i = 0; i < 20; ++i) {
        iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
        iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    }
    iOptimizationTester.stop();
    std::cout << "test-1 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-1 wins: " << iOptimizationTester.get_wins() << std::endl;
    std::cout << "test-1 losses: " << iOptimizationTester.get_losses() << std::endl;

    iOptimizationTester.calc_equity(1000, 0.8, 0.01);
    std::cout << "test-1 gain: " << iOptimizationTester.get_gain() << std::endl;
    std::vector<double> equity = iOptimizationTester.get_equity_curve();

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
    iOptimizationTester.clear();
    /* начинаем имитацию торговли */
    for(uint32_t i = 0; i < 1000; ++i) {
        double p = (double)denominator(gen)/1000.0;
        if(p <= 0.56) iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
        else iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    }
    /* завершаем торговлю */
    iOptimizationTester.stop();
    /* выводим некоторые статистические данные */
    std::cout << "test-2 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-2 wins: " << iOptimizationTester.get_wins() << std::endl;
    std::cout << "test-2 losses: " << iOptimizationTester.get_losses() << std::endl;

    iOptimizationTester.calc_equity(1000, 0.8, 0.01);
    std::cout << "test-2 gain: " << iOptimizationTester.get_gain() << std::endl;
    std::cout << "test-2 profit factor: " << iOptimizationTester.get_profit_factor() << std::endl;
    std::cout << "test-2 balance drawdown relative: " << iOptimizationTester.get_balance_drawdown_relative() << std::endl;
    std::cout << "test-2 sharpe ratio: " << iOptimizationTester.get_sharpe_ratio() << std::endl;
    std::cout << "test-2 expected payoff: " << iOptimizationTester.get_expected_payoff(0.8) << std::endl;
    std::cout << "test-2 total net profit: " << iOptimizationTester.get_total_net_profit() << std::endl;
    std::cout << "test-2 fast sharpe ratio: " << iOptimizationTester.get_fast_sharpe_ratio() << std::endl;
    return 0;
}
