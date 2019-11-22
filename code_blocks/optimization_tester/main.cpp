#include <iostream>
#include "easy_bo_optimization_tester.hpp"
#include <random>
#include <ctime>

double getAverageGeometricYield(std::vector<double> &vMoney) {
    if(vMoney.size() < 2) return 0;
    double mx = 1.0;
    for(int i = 1; i < (int)vMoney.size(); i++) {
        double ri = vMoney[i - 1] > 0 ? 1.0 + ((double)(vMoney[i] - vMoney[i - 1]) / (double)vMoney[i - 1]) : 0;
        //double ri = vMoney[i - 1] > 0 ? ((double)vMoney[i] / (double)vMoney[i - 1]) : 0;
        mx *= ri;
    }
    if(mx == 0) return 0;
    return pow(mx,(1.0/(double)vMoney.size()));
}

double getCoeffSharpe(std::vector<double> &vMoney) {
    if(vMoney.size() < 3) return 0;
    double re = getAverageGeometricYield(vMoney);
    if(re == 0) return 0;
    double sum = 0;
    for(int i = 1; i < (int)vMoney.size(); i++) {
        double ri = vMoney[i - 1] > 0 ? 1.0 + ((double)(vMoney[i] - vMoney[i - 1]) / (double)vMoney[i - 1]) : 0;
        //double ri = vMoney[i - 1] > 0 ? ((double)vMoney[i] / (double)vMoney[i - 1]) : 0;
        double diff = ri - re;
        sum += diff * diff;
    }
    if(sum == 0) return 0;
    double sigma = (1.0 / (double)(vMoney.size() - 1)) * sqrt(sum);
    //return sqrt((double)vMoney.size())*(re/sigma);
    return (re/sigma);
}

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
    std::cout << "test-1 winrate coefficient variance: " << iOptimizationTester.get_winrate_coefficient_variance() << std::endl;

    iOptimizationTester.calc_equity(1000, 0.8, 0.01);
    iOptimizationTester.calc_gross_profit_loss();
    std::cout << "test-1 gain: " << iOptimizationTester.get_gain() << std::endl;
    std::vector<double> equity = iOptimizationTester.get_equity_curve();

    for(size_t i = 0; i < equity.size(); ++i) {
        std::cout << "test-1 equity[" << i << "]: " << equity[i] << std::endl;
    }
    std::vector<double> test_equity1 = iOptimizationTester.get_equity_curve();
    std::cout << "test-1 getCoeffSharpe: " << getCoeffSharpe(test_equity1) << std::endl;
    std::cout << "test-1 getAverageGeometricYield: " << getAverageGeometricYield(test_equity1) << std::endl;
    std::cout << "test-1 calc_geometric_average_return: " << easy_bo::calc_geometric_average_return<double>(test_equity1) << std::endl;

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
        if(p <= 0.6) iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
        else iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    }
    /* завершаем торговлю */
    iOptimizationTester.stop();
    /* выводим некоторые статистические данные */
    std::cout << "test-2 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-2 wins: " << iOptimizationTester.get_wins() << std::endl;
    std::cout << "test-2 losses: " << iOptimizationTester.get_losses() << std::endl;
    std::cout << "test-2 winrate coefficient variance: " << iOptimizationTester.get_winrate_coefficient_variance() << std::endl;
    std::cout << "test-2 best3D: " << iOptimizationTester.get_coeff_best3D(1000) << std::endl;

    iOptimizationTester.calc_equity(1000, 0.8, 0.01);
    iOptimizationTester.calc_gross_profit_loss();
    std::cout << "test-2 gain: " << iOptimizationTester.get_gain() << std::endl;
    std::cout << "test-2 profit factor: " << iOptimizationTester.get_profit_factor() << std::endl;
    std::cout << "test-2 balance drawdown relative: " << iOptimizationTester.get_balance_drawdown_relative() << std::endl;
    std::cout << "test-2 sharpe ratio: " << iOptimizationTester.get_sharpe_ratio() << std::endl;
    std::cout << "test-2 expected payoff: " << iOptimizationTester.get_expected_payoff(0.8) << std::endl;
    std::cout << "test-2 total net profit: " << iOptimizationTester.get_total_net_profit() << std::endl;
    std::cout << "test-2 fast sharpe ratio: " << iOptimizationTester.get_fast_sharpe_ratio() << std::endl;

    std::vector<double> test_equity = iOptimizationTester.get_equity_curve();
    std::cout << "test-2 getCoeffSharpe: " << getCoeffSharpe(test_equity) << std::endl;
    std::cout << "test-2 getAverageGeometricYield: " << getAverageGeometricYield(test_equity) << std::endl;
    std::cout << "test-2 calc_geometric_average_return: " << easy_bo::calc_geometric_average_return<double>(test_equity) << std::endl;

    std::cout << std::endl << "start test-3" << std::endl;
    iOptimizationTester.clear();
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.stop();
    std::cout << "test-3 winrate coefficient variance: " << iOptimizationTester.get_winrate_coefficient_variance() << std::endl;
    std::cout << "test-3 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-3 best3D: " << iOptimizationTester.get_coeff_best3D(20) << std::endl;

    std::cout << std::endl << "start test-4" << std::endl;
    iOptimizationTester.clear();
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_WIN);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.add_deal(easy_bo::EASY_BO_LOSS);
    iOptimizationTester.stop();
    std::cout << "test-4 winrate coefficient variance: " << iOptimizationTester.get_winrate_coefficient_variance() << std::endl;
    std::cout << "test-4 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-4 best3D: " << iOptimizationTester.get_coeff_best3D(20) << std::endl;
    iOptimizationTester.clear();
    std::cout << std::endl << "start test-5" << std::endl;
    std::cout << "test-5 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-5 best3D: " << iOptimizationTester.get_coeff_best3D(20) << std::endl;
    iOptimizationTester.stop();
    std::cout << "test-5 winrate: " << iOptimizationTester.get_winrate<float>() << std::endl;
    std::cout << "test-5 best3D: " << iOptimizationTester.get_coeff_best3D(20) << std::endl;


    return 0;
}
