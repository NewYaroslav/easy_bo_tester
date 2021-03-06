#include <iostream>
#include "easy_bo_data_store.hpp"
#include <random>
#include <ctime>

int main(int argc, char* argv[]) {
    /* инициализируем хранилище котировок */
    std::string path = "test.dat";
    std::string fast_store_path = "test2.dat";
    if(0) {
        /* инициализируем генератор случайных чисел */
        std::mt19937 gen;
        gen.seed(time(0));
        std::uniform_int_distribution<> denominator(0, 1000);

        easy_bo::DealsDataStore iDealsDataStore(path);
        easy_bo::FastDealsDataStore iFastDealsDataStore(fast_store_path);
        size_t deals = 0;
        for(xtime::timestamp_t t = xtime::get_timestamp(1,1,2010); t <= xtime::get_timestamp(1,1,2012); t += xtime::SECONDS_IN_HOUR) {
            double p = (double)denominator(gen)/1000.0;
            if(p <= 0.6) {
                iFastDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_WIN, 180, t);
                iDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_WIN, 180, t);
            } else {
                iFastDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_LOSS, 180, t);
                iDealsDataStore.add_deals("EURUSD", 0, 0, easy_bo::EASY_BO_BUY, easy_bo::EASY_BO_LOSS, 180, t);
            }
            ++deals;
        }
        std::cout << "deals " << deals << std::endl;
    }
    {
        easy_bo::DealsDataStore iDealsDataStore(path);
        easy_bo::FastDealsDataStore iFastDealsDataStore(fast_store_path);
        std::cout << "example #1" << std::endl;
        double winrate = 0.0;
        int err = iFastDealsDataStore.get_winrate_fixed_number(winrate, 0, 0, xtime::MINUTES_IN_DAY, 100, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #2" << std::endl;
        err = iFastDealsDataStore.get_winrate_days(winrate, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #3" << std::endl;
        err = iFastDealsDataStore.get_winrate_days(winrate, 1, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #4" << std::endl;
        std::vector<float> winrate_array;
        err = iFastDealsDataStore.get_winrate_array(winrate_array, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        for(size_t i = 0; i < winrate_array.size(); ++i) {
            std::cout << "winrate[" << i << "] " << winrate_array[i] << std::endl;
        }

        std::cout << "example #5" << std::endl;
        std::vector<std::vector<float>> winrate_arrays;
        std::vector<uint32_t> symbols_index = {0,1};
        err = iFastDealsDataStore.get_winrate_arrays(winrate_arrays, symbols_index, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        for(size_t i = 0; i < winrate_arrays.size(); ++i)
        for(size_t j = 0; j < winrate_arrays[i].size(); ++j) {
            std::cout << "winrate[" << i << "][" << j << "] " << winrate_arrays[i][j] << std::endl;
        }

        std::cout << "example #6" << std::endl;
        err = iFastDealsDataStore.trade(xtime::get_timestamp(1,1,2010),[&](
                std::vector<easy_bo::DealsDataStore::Deal> &deals,
                const xtime::timestamp_t timestamp){
            std::cout << "update " << xtime::get_str_date_time(timestamp) << std::endl;
            for(size_t i = 0; i < deals.size(); ++i) {
                std::cout
                    << "name " << deals[i].name
                    << " res: " << (int)deals[i].result
                    << " t: " << xtime::get_str_date_time(timestamp) << std::endl;
            }
        });

        std::cout << "example #7" << std::endl;
        err = iFastDealsDataStore.trade(
                xtime::get_timestamp(1,1,2010),
                xtime::get_timestamp(2,1,2010),
                [&](
                std::vector<easy_bo::DealsDataStore::Deal> &deals,
                const xtime::timestamp_t timestamp){
            std::cout << "update " << xtime::get_str_date_time(timestamp) << std::endl;
            for(size_t i = 0; i < deals.size(); ++i) {
                std::cout
                    << "name " << deals[i].name
                    << " res: " << (int)deals[i].result
                    << " t: " << xtime::get_str_date_time(timestamp) << std::endl;
            }
        });

        std::cout << "example #8" << std::endl;
        std::vector<easy_bo::OneDealStruct> list_deals;
        std::vector<easy_bo::OneDealStruct> fast_list_deals;
        iDealsDataStore.get_deals_days(list_deals, 5, xtime::get_timestamp(1,1,2012));
        iFastDealsDataStore.get_deals_days(fast_list_deals, 5, xtime::get_timestamp(1,1,2012));
        if(list_deals.size() != fast_list_deals.size()) std::cout << "error, list_deals.size() != fast_list_deals.size()" << std::endl;
        for(size_t i = 0; i < list_deals.size(); ++i) {
            if(list_deals[i] != fast_list_deals[i]) std::cout << "error, list_deals[i] != fast_list_deals[i]" << std::endl;
        }

        std::cout << "example #9" << std::endl;
        const uint32_t days = 5;
        uint32_t day = 0;
        iFastDealsDataStore.process_few_days_reverse(xtime::get_timestamp(1,1,2012),[&](std::vector<easy_bo::OneDealStruct> &deals)->bool{
            if(deals.size() == 0) return true;
            std::cout << "deals: " << deals.size() << " day: " << day++ << std::endl;
            if(day >= days) return false;
            return true;
        });
    }
    std::system("pause");
    return 0;
}
