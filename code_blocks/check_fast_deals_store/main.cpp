#include <iostream>
#include "easy_bo_data_store.hpp"
#include "easy_bo_fast_storage.hpp"
#include <random>
#include <ctime>

int main(int argc, char* argv[]) {
    /* инициализируем хранилище котировок */

    std::string file_deals("deals.dat");
    easy_bo::FastDealsDataStore iDealsDataStore(file_deals);

    std::vector<easy_bo::DealsDataStore::Deal> day_stats;
    xtime::timestamp_t last_session_filter_timestamp = 0;

    int err_one = iDealsDataStore.trade(
                xtime::get_timestamp(1,1,2000),
                xtime::get_timestamp(1,1,2018),
                [&](
                std::vector<easy_bo::DealsDataStore::Deal> &deals,
                const xtime::timestamp_t timestamp) {

        if(xtime::get_first_timestamp_day(timestamp) == xtime::get_timestamp(18,2,2003)) {
            std::cout << "18.2.2003 " << deals.size() << std::endl;
        }
        if(last_session_filter_timestamp == 0) {
            last_session_filter_timestamp = xtime::get_first_timestamp_day(timestamp);
        }
        if(xtime::get_first_timestamp_day(timestamp) != last_session_filter_timestamp) {
            //if(day_stats.size() != 0) std::cout << "date: " << xtime::get_str_date(last_session_filter_timestamp) << std::endl;
            if(last_session_filter_timestamp == xtime::get_timestamp(18,2,2003)) {
                std::cout << "-18.2.2003 " << deals.size() << std::endl;
            }
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            int err_two = iDealsDataStore.get_deals(list_deals, last_session_filter_timestamp);

            if(list_deals.size() != day_stats.size()) {
                std::cout << "error size: " << list_deals.size() << " " << day_stats.size() << std::endl;
                std::system("pause");
            } else {
                for(size_t i = 0; i < list_deals.size(); ++i) {
                    if(list_deals[i] != day_stats[i]) {
                        std::cout << "error ==: " << i << std::endl;
                        std::system("pause");
                        break;
                    } else {
                       // std::cout << "OK: " << i << std::endl;
                    }
                }
            }

            last_session_filter_timestamp = xtime::get_first_timestamp_day(timestamp);
            day_stats.clear();
        }
        day_stats.insert(day_stats.end(),deals.begin(),deals.end());



    });
    std::cout << "number unique symbols: " << iDealsDataStore.get_number_unique_symbols() << std::endl;
    std::list<std::string> list_unique_symbols = iDealsDataStore.get_list_unique_symbols();
    std::for_each(list_unique_symbols.begin(), list_unique_symbols.end(), [&](const std::string &s) {
        std::cout << " " << s;
    });
    std::cout << "max index symbols: " << iDealsDataStore.get_max_index_symbols() << std::endl;

    /*
    if(0) {
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
        std::cout << "example #1" << std::endl;
        double winrate = 0.0;
        int err = iDealsDataStore.get_winrate_fixed_number(winrate, 0, 0, xtime::MINUTES_IN_DAY, 100, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #2" << std::endl;
        err = iDealsDataStore.get_winrate_days(winrate, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #3" << std::endl;
        err = iDealsDataStore.get_winrate_days(winrate, 1, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        std::cout << "winrate " << winrate << std::endl;

        std::cout << "example #4" << std::endl;
        std::vector<float> winrate_array;
        err = iDealsDataStore.get_winrate_array(winrate_array, 0, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        for(size_t i = 0; i < winrate_array.size(); ++i) {
            std::cout << "winrate[" << i << "] " << winrate_array[i] << std::endl;
        }

        std::cout << "example #5" << std::endl;
        std::vector<std::vector<float>> winrate_arrays;
        std::vector<uint32_t> symbols_index = {0,1};
        err = iDealsDataStore.get_winrate_arrays(winrate_arrays, symbols_index, 0, xtime::MINUTES_IN_DAY, 20, xtime::get_timestamp(1,1,2012));
        std::cout << "code " << err<< std::endl;
        for(size_t i = 0; i < winrate_arrays.size(); ++i)
        for(size_t j = 0; j < winrate_arrays[i].size(); ++j) {
            std::cout << "winrate[" << i << "][" << j << "] " << winrate_arrays[i][j] << std::endl;
        }

        std::cout << "example #6" << std::endl;
        err = iDealsDataStore.trade(xtime::get_timestamp(1,1,2010),[&](
                std::vector<easy_bo::DealsDataStore::Deals> &deals,
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
        err = iDealsDataStore.trade(
                xtime::get_timestamp(1,1,2010),
                xtime::get_timestamp(10,1,2010),
                [&](
                std::vector<easy_bo::DealsDataStore::Deals> &deals,
                const xtime::timestamp_t timestamp){
            std::cout << "update " << xtime::get_str_date_time(timestamp) << std::endl;
            for(size_t i = 0; i < deals.size(); ++i) {
                std::cout
                    << "name " << deals[i].name
                    << " res: " << (int)deals[i].result
                    << " t: " << xtime::get_str_date_time(timestamp) << std::endl;
            }
        });
    }
    */
    std::system("pause");
    return 0;
}
