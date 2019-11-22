#include <iostream>
#include "easy_bo_data_store.hpp"
#include <random>
#include <ctime>

int main(int argc, char* argv[]) {
    /* инициализируем хранилище котировок */
    std::string path = "test.dat";
    if(0) {
        /* инициализируем генератор случайных чисел */
        std::mt19937 gen;
        gen.seed(time(0));
        std::uniform_int_distribution<> denominator(0, 1000);

        easy_bo::DealsDataStore iDealsDataStore(path);
        size_t deals = 0;
        for(xtime::timestamp_t t = xtime::get_timestamp(1,1,2010); t <= xtime::get_timestamp(1,1,2012); t += xtime::SECONDS_IN_HOUR) {
            double p = (double)denominator(gen)/1000.0;
            if(p <= 0.6) iDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_WIN, 180, t);
            else iDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_LOSS, 180, t);
            ++deals;
        }
        std::cout << "deals " << deals << std::endl;
    }
    {
        easy_bo::DealsDataStore iDealsDataStore(path);
        double winrate = 0.0;
        int err = iDealsDataStore.get_winrate_fixed_number(winrate, 0, 0, xtime::MINUTES_IN_DAY, 100, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;
        err = iDealsDataStore.get_winrate_days(winrate, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;
        err = iDealsDataStore.get_winrate_days(winrate, 1, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::vector<float> winrate_array;
        err = iDealsDataStore.get_winrate_array(winrate_array, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        for(size_t i = 0; i < winrate_array.size(); ++i) {
            std::cout << "winrate[" << i << "] " << winrate_array[i] << std::endl;
        }
    }
    std::system("pause");
    return 0;
}
