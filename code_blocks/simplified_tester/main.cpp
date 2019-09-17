#include <iostream>
#include "easy_bo_simplifed_tester.hpp"

using namespace std;

int main()
{
    cout << "simplifed tester" << endl;
    easy_bo::SimplifedTester iSimplifedTester;
    iSimplifedTester.add_deal(easy_bo::EASY_BO_BUY);
    iSimplifedTester.add_deal(easy_bo::EASY_BO_SELL);
    iSimplifedTester.add_deal(easy_bo::EASY_BO_BUY);
    cout << "winrate: " << iSimplifedTester.get_winrate() << endl;
    cout << "clear" << endl;
    iSimplifedTester.clear();
    cout << "winrate: " << iSimplifedTester.get_winrate() << endl;

    std::vector<int8_t> delas;
    delas.reserve(100);
    for(size_t i = 0; i < 100; ++i) {
        delas.push_back(i % 3 == 0 ? easy_bo::EASY_BO_WIN : easy_bo::EASY_BO_LOSS);
    }
    cout << "+delas" << endl;
    iSimplifedTester.init(delas);
    cout << "winrate: " << iSimplifedTester.get_winrate() << endl;
    cout << "deals: " << iSimplifedTester.get_deals() << endl;

    double expect = easy_bo::calc_expectation(0.6,0.8);
    cout << "expected value: " << expect << endl;
    double min_winrate = easy_bo::calc_min_winrate(0.8);
    cout << "min winrate: " << min_winrate << endl;

    double kelly_bet = easy_bo::calc_kelly_bet(0.6, 0.8);
    cout << "kelly bet: " << kelly_bet  << endl;

    std::vector<double> depo_good = {100,110,121,133.1};
    std::vector<double> depo_bad = {100,90,81,72.9};
    std::vector<double> depo_super_bad = {100,50,0,-50};
    double geometric_average_good = easy_bo::calc_geometric_average_return(depo_good);
    double geometric_average_bad = easy_bo::calc_geometric_average_return(depo_bad);
    double geometric_average_super_bad = easy_bo::calc_geometric_average_return(depo_super_bad);
    cout << "geometric average good: " << geometric_average_good  << endl;
    cout << "geometric average bad: " << geometric_average_bad  << endl;
    cout << "geometric average super bad: " << geometric_average_super_bad  << endl;

    std::vector<double> depo_good2 = {100,120,151,133.1};
    double sharpe_ratio_good = easy_bo::calc_sharpe_ratio(depo_good2);
    double sharpe_ratio_bad = easy_bo::calc_sharpe_ratio(depo_bad);
    double sharpe_ratio_super_bad = easy_bo::calc_sharpe_ratio(depo_super_bad);
    cout << "sharpe ratio good: " << sharpe_ratio_good  << endl;
    cout << "sharpe ratio bad: " << sharpe_ratio_bad  << endl;
    cout << "sharpe ratio super bad: " << sharpe_ratio_super_bad  << endl;
    return 0;
}
