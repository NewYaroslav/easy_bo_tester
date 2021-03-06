#include <iostream>
#include "easy_bo_data_store.hpp"
#include "easy_bo_fast_storage.hpp"
#include <random>
#include <ctime>
#include <cstdio>

int main(int argc, char* argv[]) {
    std::string path = "test.dat";
    /* сначала удалаяем хранилище */
    std::remove(path.c_str());

    /* создаем хранилище и записываем сделки */
    std::cout << "step-1" << std::endl;
    {
        easy_bo::FastDealsDataStore store(path);

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

        std::cout << "add deals" << std::endl;
        int err_add = 0;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(1,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(1,1,2019,0,0,0)) << std::endl;
        err_add = store.add_deal("EURUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(1,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("AUDCAD", 0, 0, 1, 1, 180, xtime::get_timestamp(1,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("GBPUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(1,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(1,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(2,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(2,1,2019,0,0,0)) << std::endl;
        err_add = store.add_deal("EURUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(2,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("AUDCAD", 0, 0, 1, 1, 180, xtime::get_timestamp(2,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("GBPUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(2,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(2,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(3,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(3,1,2019,0,0,0)) << std::endl;
        err_add = store.add_deal("EURUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("AUDCAD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("GBPUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, -1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        std::cout << "save" << std::endl;
        int err_save = store.save();
        if(err_save != easy_bo::OK) std::cout << "err_save " << err_save << std::endl;

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

    }
    std::cout << "step-2" << std::endl;
    /* прочитаем */
    {
        easy_bo::FastDealsDataStore store(path);

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

        std::cout << "1,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(1,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(1,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(1,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }
        std::cout << "2,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(2,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(2,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(2,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }
        std::cout << "3,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(3,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(3,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(3,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

    }
    std::cout << "step-3" << std::endl;
    /* добавим еще */
    {
        easy_bo::FastDealsDataStore store(path);

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

        std::cout << "add deals" << std::endl;
        int err_add = 0;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(3,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(3,1,2019,0,0,0)) << std::endl;
        err_add = store.add_deal("EURUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("AUDCAD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("GBPUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, -1, 180, xtime::get_timestamp(3,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;

        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(4,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(4,1,2019,0,0,0)) << std::endl;
        err_add = store.add_deal("EURUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(4,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("AUDCAD", 0, 0, 1, 1, 180, xtime::get_timestamp(4,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("GBPUSD", 0, 0, 1, 1, 180, xtime::get_timestamp(4,1,2019,1,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;
        err_add = store.add_deal("NZDJPY", 0, 0, 1, 1, 180, xtime::get_timestamp(4,1,2019,0,0,0)); if(err_add != easy_bo::OK) std::cout << "err_add " << err_add << std::endl;

        std::cout << "save" << std::endl;
        int err_save = store.save();
        if(err_save != easy_bo::OK) std::cout << "err_save " << err_save << std::endl;

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }
    }
    std::cout << "step-4" << std::endl;
    /* прочитаем */
    {
        easy_bo::FastDealsDataStore store(path);

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

        std::cout << "1,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(1,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(1,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(1,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }
        std::cout << "2,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(2,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(2,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(2,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }
        std::cout << "3,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(3,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(3,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(3,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }
        std::cout << "4,1,2019" << std::endl;
        std::cout << "check " <<  xtime::get_str_date(xtime::get_timestamp(4,1,2019,0,0,0)) << " " << store.check_timestamp(xtime::get_timestamp(4,1,2019,0,0,0)) << std::endl;
        {
            std::vector<easy_bo::FastDealsDataStore::Deal> list_deals;
            store.get_deals(list_deals, xtime::get_timestamp(4,1,2019,0,0,0));
            for(size_t i = 0; i < list_deals.size(); ++i) {
                std::cout
                    << list_deals[i].get_name() << " "
                    << (int)list_deals[i].symbol << " "
                    << (int)list_deals[i].group << " "
                    << (int)list_deals[i].direction << " "
                    << (int)list_deals[i].result << " "
                    << (int)list_deals[i].duration << " "
                    << xtime::get_str_date_time(list_deals[i].timestamp) << std::endl;
            }
        }

        {
            xtime::timestamp_t deals_timestamp_min = 0, deals_timestamp_max = 0;
            int err = store.get_min_max_timestamp(deals_timestamp_min, deals_timestamp_max);
            if(err == easy_bo::OK) {
                std::cout << "date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            } else {
                std::cout << "error date: " << xtime::get_str_date(deals_timestamp_min) << " " << xtime::get_str_date(deals_timestamp_max) << std::endl;
            }
        }

    }

#if(0)
    easy_bo::ArrayDealsStorage iArrayDealsStorage(path);
    easy_bo::ArrayDeals iArrayDeals;

    easy_bo::OneDealStruct iOneDealStruct;
    iOneDealStruct.direction= 1;
    iOneDealStruct.result = 1;
    iOneDealStruct.duration = 180;
    iOneDealStruct.symbol = 0;
    iOneDealStruct.timestamp = xtime::get_timestamp(1,1,2012);
    iArrayDeals.add_deal(iOneDealStruct);

    iOneDealStruct.timestamp = xtime::get_timestamp(2,1,2012);
    iArrayDeals.add_deal(iOneDealStruct);

    iOneDealStruct.timestamp = xtime::get_timestamp(3,1,2012);
    iArrayDeals.add_deal(iOneDealStruct);

    std::cout << "number unique symbols: " << iDealsDataStore.get_number_unique_symbols() << std::endl;
#endif

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
