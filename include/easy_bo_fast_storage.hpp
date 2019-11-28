#ifndef EASY_BO_FAST_STORAGE_HPP_INCLUDED
#define EASY_BO_FAST_STORAGE_HPP_INCLUDED

#include "xquotes_daily_data_storage.hpp"
#include <cstring>

namespace easy_bo {
    static const size_t NAME_SIZE = 32;

    /** \brief Класс для хранения всех объектов одной сделки
     */
    class OneDealStruct {
    public:
        int8_t name[NAME_SIZE] = {};        /**< Имя символа, ограничено 31-байтом */
        xtime::timestamp_t timestamp = 0;   /**< Метка времени начала бинарного опицона */
        uint32_t duration = 0;              /**< Продолжительность бинарного опциона в секундах */
        float winrate = 0;                  /**< Прогноз винрейта */
        int8_t direction = EASY_BO_NO_BET;  /**< Направление ставки, покупка или продажа опциона */
        int8_t result = EASY_BO_NEUTRAL;    /**< Результат опциона, победа или поражение */
        uint8_t group = 0;					/**< Группа сделок */
        uint8_t symbol = 0;					/**< Индекс символа */

        OneDealStruct() {};

        bool operator == (const OneDealStruct &b) {
            return ( timestamp == b.timestamp &&
                duration == b.duration &&
                direction == b.direction &&
                result == b.result &&
                group == b.group &&
                symbol == b.symbol &&
                std::equal(name, name + NAME_SIZE, b.name));
        }

        bool operator != (const OneDealStruct &b) {
            return ( timestamp != b.timestamp ||
                duration != b.duration ||
                direction != b.direction ||
                result != b.result ||
                group != b.group ||
                symbol != b.symbol ||
                !std::equal(name, name + NAME_SIZE, b.name));
        }

        /** \brief Получить имя символа
         * \return Имя символа
         */
        std::string get_name() const {
            return std::string(
                (const char*)name,
                std::strlen((const char*)name));
        }

        /** \brief Установить имя символа
         * \param symbol_name Имя символа
         */
        void set_name(const std::string &symbol_name) {
            const size_t min_size = std::min(symbol_name.length(), NAME_SIZE);
            std::strncpy((char*)name, (const char*)symbol_name.c_str(), min_size);
        }
    };

    class ArrayDeals {
    private:
        std::unique_ptr<uint8_t[]> byte_array;  /**< Массив с данными */
        size_t byte_array_size = 0;
        uint8_t *byte_array_ptr = nullptr;
    public:
        enum {
            COMPARE_TIMESTAMP = 0,
        };

        ArrayDeals() {};

        /* данный код не изменять! он должен остаться без изменений, т.к. нужен для
         * работы с хранилищем данных.
         */
        virtual ArrayDeals & operator = (const ArrayDeals &array_deals) {
            if(this != &array_deals) {
                if(byte_array_size != array_deals.byte_array_size) {
                    byte_array_size = array_deals.byte_array_size;
                    byte_array = std::unique_ptr<uint8_t[]>(new uint8_t[byte_array_size]);
                }
                uint8_t *dst_data = byte_array.get();
                uint8_t *src_data = array_deals.byte_array.get();
                std::copy(src_data, src_data + byte_array_size, dst_data);
                byte_array_ptr = byte_array.get();
            }
            return *this;
        }

        ArrayDeals(const ArrayDeals &array_deals) {
            if(this != &array_deals) {
                if(byte_array_size != array_deals.byte_array_size) {
                    byte_array_size = array_deals.byte_array_size;
                    byte_array = std::unique_ptr<uint8_t[]>(new uint8_t[byte_array_size]);
                }
                uint8_t *dst_data = byte_array.get();
                uint8_t *src_data = array_deals.byte_array.get();
                std::copy(src_data, src_data + byte_array_size, dst_data);
                byte_array_ptr = byte_array.get();
            }
        }

        virtual bool empty() {
            return (byte_array_size == 0);
        }

        virtual void assign(const char* s, size_t n) {
            if(n != byte_array_size) {
                byte_array = std::unique_ptr<uint8_t[]>(new uint8_t[n]);
                byte_array_size = n;
            }
            char *point = (char*)byte_array.get();
            std::copy(s, s + n, point);
            byte_array_ptr = byte_array.get();
        }

        virtual size_t size() const {
            return byte_array_size;
        }

        virtual const char* data() const noexcept {
            return (const char*)byte_array.get();
        }

        /* дальше пользовательские методы/конструкторы */

        /** \brief Получить количество сделок
         * \return Количество сделок
         */
        inline uint32_t get_amount_deals() {
            return byte_array_size / sizeof(OneDealStruct);
        }

        ArrayDeals(const uint32_t deals) {
            set_amount_deals(deals);
        }

        private:

        /** \brief Проверка отсортированности по метке времени
         */
        bool is_sorted_timestamp() {
            const uint32_t deals = get_amount_deals();
            OneDealStruct *value_ptr = (OneDealStruct*)byte_array_ptr;
            for(uint32_t d = 1; d < deals; ++d) {
                if(value_ptr[d].timestamp < value_ptr[d - 1].timestamp) return false;
            }
            return true;
        }

        /** \brief Быстрая сортировка Хоара. Сортирует массив по возрастанию
         *
         * \param a массив, который нужно отсортировать
         * \param l индекс первого элемента массива
         * \param r индекс последнего элемента массива (это не размер массива!)
         */
        template<const int type_comp, typename T>
        void quick_sort(T* a, long l, long r) {
            long i = l, j = r;
            T temp, p;
            p = a[l + (r - l)/2];
            do {
                switch(type_comp) {
                case COMPARE_TIMESTAMP:
                    while(a[i].timestamp < p.timestamp) i++;
                    while(a[j].timestamp > p.timestamp) j--;
                    break;
                }

                if(i<= j) {
                    temp = a[i];
                    a[i] = a[j];
                    a[j] = temp;
                    i++; j--;
                }
            } while (i <=j);
            if(i < r)
                quick_sort<type_comp, T>(a, i, r);
            if(l < j)
                quick_sort<type_comp, T>(a, l , j);
        };

        template<class T>
        size_t binary_search_timestamp(
                T* data,
                T keyData,
                size_t left,
                size_t right) {
            size_t low, high, mid;
            low = left;
            if(left >= (right + 1)) {
                high = left;
            } else {
                high = right + 1;
            }
            while(low < high) {
                mid = (low + high) / 2;
                if (keyData <= data[mid]) {
                    high = mid;
                } else {
                    low = mid + 1;
                }
            }
            return high;
        }

        /** \brief Сортировка сделок по метке времени
         */
        void sort_timestamp() {
            if(is_sorted_timestamp()) return;
            const uint32_t deals = get_amount_deals();
            OneDealStruct *deals_ptr = (OneDealStruct*)byte_array_ptr;
            quick_sort<COMPARE_TIMESTAMP, OneDealStruct>(deals_ptr, 0, deals - 1);
        }

        public:

        /** \brief Установить количество сделок
         * \param deals Количество сделок
         */
        void set_amount_deals(const uint32_t deals) {
            const size_t old_size = byte_array_size;
            const size_t new_size = deals * sizeof(OneDealStruct);
            if(new_size != byte_array_size) {
                byte_array = std::unique_ptr<uint8_t[]>(new uint8_t[new_size]);
                byte_array_size = new_size;
                byte_array_ptr = byte_array.get();
                std::fill(byte_array_ptr + old_size, byte_array_ptr + byte_array_size, '\0');
            }
        }

        /** \brief Получить сделку по индексу
         * \return Ссылка на сделку
         */
        OneDealStruct &get_deal(const uint32_t deal_index) {
            OneDealStruct *value_ptr = (OneDealStruct*)byte_array_ptr;
            return value_ptr[deal_index];
        }

        /** \brief Установить сделку по индексу
         * \param deal Сделка
         * \param deal_index Индекс сделки
         */
        void set_deal(const OneDealStruct &deal, const uint32_t deal_index) {
            OneDealStruct *value_ptr = (OneDealStruct*)byte_array_ptr;
            value_ptr[deal_index] = deal;
        }

        /** \brief Добавить сделку
         * \param deal Сделка
         */
        void add_deal(const OneDealStruct &deal) {
            const uint32_t deals = get_amount_deals();
            set_amount_deals(deals + 1);
            set_deal(deal, deals);
        }

        /** \brief Получить вектор сделок
         * \param deals Массив слелок
         */
        void get_vector(std::vector<OneDealStruct> &deals) {
            OneDealStruct *value_ptr = (OneDealStruct*)byte_array_ptr;
            const uint32_t deals_count = get_amount_deals();
            deals.clear();
            deals.assign(value_ptr, value_ptr + deals_count);
        }

        /** \brief Получить вектор сделок
         * \param deals Массив слелок
         */
        void set_vector(const std::vector<OneDealStruct> &deals) {
            set_amount_deals(deals.size());
            OneDealStruct *deals_ptr = (OneDealStruct*)byte_array_ptr;
            for(size_t i = 0; i < deals.size(); ++i) {
                deals_ptr[i] = deals[i];
            }
        }
    };

    /** \brief Класс для хранения статистики сделок с быстрым доступом к сделкам
     */
    class DealsFastStorage :
        public xquotes_daily_data_storage::DailyDataStorage<ArrayDeals> {
    private:
        typedef OneDealStruct Deal;
        std::vector<Deal> list_write_deals;	/**< Массив сделок */
    public:
        DealsFastStorage() : DailyDataStorage() {};

        DealsFastStorage(
            const std::string &path,
            const char* dictionary_ptr = NULL,
            const size_t dictionary_size = 0) : DailyDataStorage(path, dictionary_ptr, dictionary_size) {
        };


    };

    typedef xquotes_daily_data_storage::
        DailyDataStorage<ArrayDeals> ArrayDealsStorage; /**< Хранилище массива сделок */
};

#endif // EASY_BO_FAST_STORAGE_HPP_INCLUDED
