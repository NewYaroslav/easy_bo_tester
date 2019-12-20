/*
* easy_bo_tester - C++ header-only library for testing binary options
*
* Copyright (c) 2018 Elektro Yar. Email: git.electroyar@gmail.com
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
* SOFTWARE.
*/
#ifndef EASY_BO_DATA_STORE_HPP_INCLUDED
#define EASY_BO_DATA_STORE_HPP_INCLUDED

#include <vector>
#include <algorithm>
#include <limits>
#include <type_traits>
#include <list>

#include "easy_bo_common.hpp"
#include "easy_bo_simplifed_tester.hpp"
#include "easy_bo_fast_storage.hpp"
#include "xquotes_json_storage.hpp"
#include "xtime.hpp"

namespace easy_bo {

	/** \brief Класс хранилища сделок
	 */
    template<class STORAGE_TYPE = xquotes_json_storage::JsonStorage>
	class DealsDataStoreTemplate {
	public:

#if(0)
		/** \brief Класс для хранения сделки
		 */
		class Deal {
		public:
			xtime::timestamp_t timestamp = 0;   /**< Метка времени начала бинарного опицона */
			uint32_t duration = 0;              /**< Продолжительность бинарного опциона в секундах */
			int8_t direction = EASY_BO_NO_BET;  /**< Направление ставки, покупка или продажа опциона */
			int8_t result = EASY_BO_NEUTRAL;    /**< Результат опциона, победа или поражение */
			uint8_t group = 0;					/**< Группа сделок */
			uint8_t symbol = 0;					/**< Индекс символа */
			std::string name;					/**< Имя символа */
			Deal() {};

			bool operator == (const Deal &b) {
				return ( timestamp == b.timestamp &&
                    duration == b.duration &&
                    direction == b.direction &&
                    result == b.result &&
                    group == b.group &&
                    symbol == b.symbol &&
                    name == b.name);
			}
		};
#endif
    typedef OneDealStruct Deal;

	private:
		std::vector<Deal> list_write_deals;	/**< Массив сделок */
		STORAGE_TYPE iStorage;	                /**< Хранилище данных сделок, разбитых по дням */
        xtime::timestamp_t date_timestamp = 0;  /**< Метка времени начала исторических данных */

		/** \brief Записать сделки за один торговый день
         * \param list_deals Список сделок
         * \param timestamp Метка времени
         * \return Вернет 0 в случае успеха
         */
        template<typename T, typename std::enable_if<std::is_same<T, xquotes_json_storage::JsonStorage>::value>::type* = nullptr>
        int write_deals(const std::vector<Deal> &list_deals, const xtime::timestamp_t timestamp) {
            nlohmann::json j;
            for(size_t i = 0; i < list_deals.size(); ++i) {
                j[i]["name"] = list_deals[i].get_name();
                j[i]["symbol"] = list_deals[i].symbol;
                j[i]["result"] = list_deals[i].result;
                j[i]["direction"] = list_deals[i].direction;
                j[i]["duration"] = list_deals[i].duration;
				j[i]["group"] = list_deals[i].group;
				j[i]["timestamp"] = list_deals[i].timestamp;
            }
            return iStorage.write_json(j, xtime::get_first_timestamp_day(timestamp));
        }

        /** \brief Записать сделки за один торговый день
         * \param list_deals Список сделок
         * \param timestamp Метка времени
         * \return Вернет 0 в случае успеха
         */
        template<typename T, typename std::enable_if<std::is_same<T, ArrayDealsStorage>::value>::type* = nullptr>
        int write_deals(const std::vector<Deal> &list_deals, const xtime::timestamp_t timestamp) {
            ArrayDeals iArrayDeals;
            iArrayDeals.set_vector(list_deals);
            return iStorage.write_day_data(iArrayDeals, xtime::get_first_timestamp_day(timestamp));
        }

		/** \brief Прочитать сделки за торговый день
         * \param list_deals Массив сделок
         * \param timestamp Метка времени
         * \return Вернет 0 в случае успеха
         */
        template<typename T, typename std::enable_if<std::is_same<T, xquotes_json_storage::JsonStorage>::value>::type* = nullptr>
        int read_deals(std::vector<Deal> &list_deals, const xtime::timestamp_t timestamp) {
            list_deals.clear();
            nlohmann::json j;
            try {
                int err = iStorage.get_json(j, xtime::get_first_timestamp_day(timestamp));
                if(err != xquotes_common::OK) return err;
                list_deals.resize(j.size());
                for(size_t i = 0; i < list_deals.size(); ++i) {
                    std::string name = j[i]["name"];
                    list_deals[i].set_name(name);
                    list_deals[i].symbol = j[i]["symbol"];
                    list_deals[i].result = j[i]["result"];
                    list_deals[i].direction = j[i]["direction"];
					list_deals[i].duration = j[i]["duration"];
					list_deals[i].group = j[i]["group"];
                    list_deals[i].timestamp = j[i]["timestamp"];
                }
            }
            catch(...) {
                return PARSER_ERROR;
            }
            return OK;
        }

        /** \brief Прочитать сделки за торговый день
         * \param list_deals Массив сделок
         * \param timestamp Метка времени
         * \return Вернет 0 в случае успеха
         */
        template<typename T, typename std::enable_if<std::is_same<T, ArrayDealsStorage>::value>::type* = nullptr>
        int read_deals(std::vector<Deal> &list_deals, const xtime::timestamp_t timestamp) {
            list_deals.clear();
            ArrayDeals iArrayDeals;
            int err = iStorage.get_day_data(iArrayDeals, xtime::get_first_timestamp_day(timestamp));
            if(err != xquotes_common::OK) return err;
            iArrayDeals.get_vector(list_deals);
            return OK;
        }

		/** \brief Сортировка массива сделок
         */
        void sort_list_deals(std::vector<Deal> &list_deals) {
            if(!std::is_sorted(list_deals.begin(), list_deals.end(),
                [](const Deal &a, const Deal &b) {
                        return a.timestamp < b.timestamp;
                    })) {
                std::sort(list_deals.begin(), list_deals.end(),
                [](const Deal &a, const Deal &b) {
                    return a.timestamp < b.timestamp;
                });
            }
        }

		/** \brief Найти сделку по метке времени
         * \return указатель на сделку с похожей меткой времени
         */
        typename std::vector<Deal>::iterator find_deal(std::vector<Deal> &list_deals, const xtime::timestamp_t timestamp) {
            if(list_deals.size() == 0) return list_deals.end();
            auto list_deals_it = std::lower_bound(
                list_deals.begin(),
                list_deals.end(),
                timestamp,
                [](const Deal &lhs, const xtime::timestamp_t &timestamp) {
                return lhs.timestamp < timestamp;
            });
            if(list_deals_it == list_deals.end()) {
                return list_deals.end();
            } else
            if(list_deals_it->timestamp == timestamp) {
                return list_deals_it;
            }
            return list_deals.end();
        }

		bool check_repeat_deal(std::vector<Deal> &list_deals, const Deal &deal) {
			auto it = find_deal(list_deals, deal.timestamp);
			if(it == list_deals.end()) return false;
			const size_t start_offset = it - list_deals.begin();
			for(size_t i = start_offset; i < list_deals.size(); ++i) {
				if(list_deals[i] == deal) return true;
				if(list_deals[i].timestamp > deal.timestamp) break;
			}
			return false;
		}

	public:
		/** \brief Инициализировать базу данных новостей
         * \param _path путь к базе данных
         */
        DealsDataStoreTemplate(const std::string &path) : iStorage(path) {};

		/** \brief Проверить наличие новостей за торговый день по метке времени
         * \param timestamp метка времени
         * \return вернет true если файл есть
         */
        bool check_timestamp(const xtime::timestamp_t timestamp) {
            return iStorage.check_timestamp(timestamp);
        }

        /** \brief Узнать максимальную и минимальную метку времени подфайлов
         * \param min_timestamp Метка времени в начале дня начала исторических данных
         * \param max_timestamp Метка времени в начале дня конца исторических данных
         * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
        int get_min_max_timestamp(xtime::timestamp_t &min_timestamp, xtime::timestamp_t &max_timestamp) {
            return iStorage.get_min_max_timestamp(min_timestamp, max_timestamp);
        }

		/** \brief Установить отступ данных от дня загрузки
         *
         * Отсутп позволяет загрузить используемую область данных заранее.
         * Это позволит получать данные в пределах области без повторной загрузки подфайлов.
         * При этом, цесли данные выйдут за пределы области, то область сместится, произойдет подзагрузка
         * недостающих данных.
         * \param indent_timestamp_past Отступ от даты загрузки в днях к началу исторических данных
         * \param indent_timestamp_future Отступ от даты загрузки в днях к концу исторических данных
         */
        void set_indent(
                const uint32_t indent_timestamp_past,
                const uint32_t indent_timestamp_future) {
            iStorage.set_indent(indent_timestamp_past, indent_timestamp_future);
        }

        /** \brief Сохранить данные
         *
         * Метод  принудительно сохраняет все данные, которые еще не записаны в файл а находятся только в буфере.
         */
        int save() {
			if(list_write_deals.size() > 0 && date_timestamp != 0) {
				/* данные уже есть! Придется сначала прочитать старые данные */
				std::vector<Deal> temp;
				int err = read_deals<STORAGE_TYPE>(temp, date_timestamp);
				if(err != xquotes_common::OK) {
					iStorage.save();
					return err;
				}
				/* добавим во временный массив не повторяющиеся сделки */
				if(temp.size() > 0) {
					for(size_t i = 0; i < list_write_deals.size(); ++i) {
						if(!check_repeat_deal(temp, list_write_deals[i])) {
							temp.push_back(list_write_deals[i]);
							/* ужасно не оптимальный код */
							sort_list_deals(temp);
						}
					}
				} else {
					temp = list_write_deals;
					if(temp.size() > 0) sort_list_deals(temp);
				}
				/* а теперь все обратно запишем */
				err = write_deals<STORAGE_TYPE>(temp, date_timestamp);
				if(err != xquotes_common::OK) {
					iStorage.save();
					return err;
				}
			}
            iStorage.save();
            return OK;
        }

        /** \brief Получить сделки за указанный торговый день
         *
         * \param list_deals Массив сделок
         * \param timestamp Дата
         * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
         * \return Вернет код ошибки
         */
        int get_deals(
                std::vector<Deal> &list_deals,
                const xtime::timestamp_t timestamp,
                std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
            if(!check_timestamp(timestamp)) return easy_bo::NO_DATA_ACCESS;
            int err = read_deals<STORAGE_TYPE>(list_deals, timestamp);
            if(err != easy_bo::OK) return err;
            sort_list_deals(list_deals);
            if(callback != nullptr) callback(list_deals);
            if(list_deals.size() == 0) return easy_bo::NO_DATA_ACCESS;
            return easy_bo::OK;
        }

        /** \brief Торгуем день
         *
         * \param timestamp Дата
         * \param callback Лямбда-функция для обратного вызова.
         * Она принимает массивы сделок по метке времени
         * \param step Шаг времени
         * \return Код ошибки
         */
        int trade(
                const xtime::timestamp_t timestamp,
                std::function<void(
                    std::vector<Deal> &deals,
                    const xtime::timestamp_t timestamp)> callback,
                const xtime::timestamp_t step = xtime::SECONDS_IN_MINUTE) {

            const xtime::timestamp_t start = xtime::get_first_timestamp_day(timestamp);
            if(!check_timestamp(start)) return easy_bo::NO_DATA_ACCESS;
            std::vector<Deal> list_deals;
            int err = read_deals<STORAGE_TYPE>(list_deals, start);
            if(err != easy_bo::OK) return err;
            sort_list_deals(list_deals);
            std::vector<Deal> temp;
            temp.reserve(list_deals.size());
            const xtime::timestamp_t stop = start + xtime::SECONDS_IN_DAY;
            for(xtime::timestamp_t t = start; t < stop; t += step) {
                /* формируем список для каждой минуты дня */
                auto it = find_deal(list_deals, t);
                if(it == list_deals.end()) continue;
                temp.clear();
                const size_t offset = it - list_deals.begin();
                for(size_t i = offset; i < list_deals.size(); ++i) {
                    if(list_deals[i].timestamp == t) temp.push_back(list_deals[i]);
                }
                if(temp.size() > 0) callback(temp, t);
            }
            return easy_bo::OK;
        }

        /** \brief Торговать за указанный период
         *
         * \param start_date_timestamp Начальная дата
         * \param stop_date_timestamp Конечная дата
         * \param callback Обработчик сделок
         * \param step Шаг времени внутри дня, по умолчанию минута
         * \return Вернет 0 если были данные
         */
        int trade(
                const xtime::timestamp_t start_date_timestamp,
                const xtime::timestamp_t stop_date_timestamp,
                std::function<void(
                        std::vector<Deal> &deals,
                        const xtime::timestamp_t timestamp)> callback,
                const xtime::timestamp_t step = xtime::SECONDS_IN_MINUTE) {
            const xtime::timestamp_t end_timestamp = xtime::get_first_timestamp_day(stop_date_timestamp);
            int counter = 0;
            for(xtime::timestamp_t t = xtime::get_first_timestamp_day(start_date_timestamp);
                t <= end_timestamp;
                t += xtime::SECONDS_IN_DAY) {
                int err = trade(t, callback, step);
                if(err == easy_bo::OK) ++counter;
            }
            if(counter == 0) return easy_bo::NO_DATA_ACCESS;
            return easy_bo::OK;
        }

        /** \brief Обработать сделки за несколько дней
         *
		 * Метод удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int process_few_days_reverse(
				const xtime::timestamp_t stop_timestamp,
				std::function<bool(std::vector<Deal> &deals)> callback) {
			xtime::timestamp_t min_timestamp = 0;
			xtime::timestamp_t max_timestamp = 0;
			int err = get_min_max_timestamp(min_timestamp, max_timestamp);
			if(err != xquotes_common::OK) return err;
			xtime::timestamp_t timestamp = xtime::get_first_timestamp_day(stop_timestamp) - xtime::SECONDS_IN_DAY;
			const xtime::timestamp_t protection_timestamp = xtime::get_last_timestamp_day(timestamp);
			while(timestamp >= min_timestamp) {
				/* проверяем доступность данных за требуемую дату */
				if(!check_timestamp(timestamp)) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}
				/* загружаем данные за торговый день */
				std::vector<Deal> temp;
				err = read_deals<STORAGE_TYPE>(temp, timestamp);
				if(err != xquotes_common::OK) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}
				/* удаляем те сделки, которые выходят
				 * за максимально допустимую дату
				 */
				size_t temp_index = 0;
				while(temp_index < temp.size()) {
					const xtime::timestamp_t last_timestamp =
						temp[temp_index].timestamp + temp[temp_index].duration;
					if(last_timestamp > protection_timestamp) {
						temp.erase(temp.begin() + temp_index);
						continue;
					}
					++temp_index;
				}
				if(!callback(temp)) break;
				timestamp -= xtime::SECONDS_IN_DAY;
			}
			return OK;
		}

		/** \brief Получить сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param list_deals Массив сделок
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_deals_days(
				std::vector<Deal> &list_deals,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			list_deals.clear();
			xtime::timestamp_t min_timestamp = 0;
			xtime::timestamp_t max_timestamp = 0;
			int err = get_min_max_timestamp(min_timestamp, max_timestamp);
			if(err != xquotes_common::OK) return err;
			xtime::timestamp_t timestamp = xtime::get_first_timestamp_day(stop_timestamp) - xtime::SECONDS_IN_DAY;
			const xtime::timestamp_t protection_timestamp = xtime::get_last_timestamp_day(timestamp);
			uint32_t day = 0;
			while(day < days && timestamp >= min_timestamp) {
				/* проверяем доступность данных за требуемую дату */
				if(!check_timestamp(timestamp)) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}
				/* загружаем данные за торговый день */
				std::vector<Deal> temp;
				err = read_deals<STORAGE_TYPE>(temp, timestamp);
				if(err != xquotes_common::OK) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}
				/* обрабатываем данные */
				size_t temp_index = 0;
				while(temp_index < temp.size()) {
					const xtime::timestamp_t last_timestamp =
						temp[temp_index].timestamp + temp[temp_index].duration;
					if(last_timestamp > protection_timestamp) {
						temp.erase(temp.begin() + temp_index);
						continue;
					}
					++temp_index;
				}
				if(temp.size() != 0 && callback != nullptr) callback(temp);
				/* пропускаем этот день без сделок */
				if(temp.size() == 0) {
                    timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}

				/* добавлям данные в общий вектор */
				if(temp.size() > 0) list_deals.insert(list_deals.end(), temp.begin(), temp.end());

				timestamp -= xtime::SECONDS_IN_DAY;
				++day;
			}
			if(day < days) {
				list_deals.clear();
				return NO_DATA_ACCESS;
			}
			if(list_deals.size() == 0) return NO_DATA_ACCESS;
			/* сортируем данные */
			sort_list_deals(list_deals);
			return OK;
		}

		/** \brief Получить сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param list_deals Массив сделок
		 * \param symbol_index Индекс символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_deals_days(
				std::vector<Deal> &list_deals,
				const uint32_t symbol_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			return get_deals_days(
				list_deals,
				days,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* удаляем ненужные данные */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						if(symbol_index != temp[temp_index].symbol) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					if(temp.size() != 0 && callback != nullptr) callback(temp);
				});
		}

		/** \brief Получить последние сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param list_deals Массив сделок
		 * \param symbol_name Имя символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_deals_days(
				std::vector<Deal> &list_deals,
				const std::string &symbol_name,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			return get_deals_days(
				list_deals,
				days,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* удаляем ненужные данные */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						if(symbol_name != temp[temp_index].get_name()) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					if(temp.size() != 0 && callback != nullptr) callback(temp);
				});
		}

		/** \brief Получить сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param list_deals Массив сделок
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_deals_days(
				std::vector<Deal> &list_deals,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			return get_deals_days(
				list_deals,
				days,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* удаляем ненужные данные */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					if(temp.size() != 0 && callback != nullptr) callback(temp);
				});
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate Винрейт
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_days(
				T &winrate,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_deals_days(list_deals, days, stop_timestamp, callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate Винрейт
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_days(
				T &winrate,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_deals_days(
				list_deals,
				start_minute_day,
				stop_minute_day,
				days,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate Винрейт
		 * \param symbol_index Индекс символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_days(
				T &winrate,
				const uint32_t symbol_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_deals_days(
				list_deals,
				symbol_index,
				start_minute_day,
				stop_minute_day,
				days,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить винрейт за указанное количество последних дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate Винрейт
		 * \param symbol_name Имя символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_days(
				T &winrate,
				const std::string &symbol_name,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_deals_days(
				list_deals,
				symbol_name,
				start_minute_day,
				stop_minute_day,
				days,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить массив винрейтов за указанное количество дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate_array Массив винрейтов
		 * \param symbol_index Индекс символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_array(
				std::vector<T> &winrate_array,
				const uint32_t symbol_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			std::vector<Deal> list_deals;
			int err = get_deals_days(
				list_deals,
				symbol_index,
				start_minute_day,
				stop_minute_day,
				days,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			winrate_array.resize(days);
			easy_bo::SimplifedTester<uint32_t> tester;
			uint32_t day = xtime::get_day(list_deals[0].timestamp);
			size_t index = 0;
			for(size_t i = 0; i < list_deals.size(); ++i) {
                const uint32_t index_day = xtime::get_day(list_deals[i].timestamp);
                if(index_day != day) {
                    winrate_array[index] = tester.get_winrate<T>();
                    ++index;
                    tester.clear();
                    day = index_day;
                }
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate_array[index] = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить массивы винрейтов за указанное количество дней
         *
         * Данный метод получит винрейт вектора сделок за указанное количество дней. Текущий день не учитывается.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" за последнюю дату.
         * \param winrate_array Массив винрейтов нескольких символов
		 * \param symbols_index Массив индексов символов
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param days Количество дней
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_arrays(
				std::vector<std::vector<T>> &winrate_arrays,
				const std::vector<uint32_t> &symbols_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t days,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
            winrate_arrays.resize(symbols_index.size(),std::vector<T>(days));
            easy_bo::SimplifedTester<uint32_t> tester;
            size_t day = 0;
			std::vector<Deal> list_deals; // не будет использован
			int err = get_deals_days(
				list_deals,
				days,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* получаем данные за один день
					 * и сначала удалим из данных лишние минуты
					 */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					/* далее, если еще что-то осталось от данных,
					 * вызовем пользователькую функцию фильтра
					 */
					if(temp.size() != 0 && callback != nullptr) callback(temp);
					if(temp.size() != 0) {
                        /* далее обработаем все символы */
                        for(size_t symbol_index = 0; symbol_index < symbols_index.size(); ++symbol_index) {
                            tester.clear();
                            for(size_t i = 0; i < temp.size(); ++i) {
                                if(symbol_index != temp[i].symbol) continue;
                                if(temp[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
                                else tester.add_deal(easy_bo::EASY_BO_LOSS);
                            }
                            winrate_arrays[symbol_index][day] = tester.get_winrate<T>();
                        }
                        ++day;
                    }
				});
            if(err == OK) {
                for(size_t i = 0; i < winrate_arrays.size(); ++i) {
                    std::reverse(winrate_arrays[i].begin(), winrate_arrays[i].end());
                }
            }
            return err;
		}

		/** \brief Получить фиксированное количество сделок
         *
         * * Данный метод загрузит вектор сделок заданного размера.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param list_deals Массив сделок
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_fixed_number_deals(
				std::vector<Deal> &list_deals,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			list_deals.clear();
			xtime::timestamp_t min_timestamp = 0;
			xtime::timestamp_t max_timestamp = 0;
			int err = get_min_max_timestamp(min_timestamp, max_timestamp);
			if(err != xquotes_common::OK) return err;
			xtime::timestamp_t timestamp = xtime::get_first_timestamp_day(stop_timestamp);
			const xtime::timestamp_t protection_timestamp = stop_timestamp;
			while(timestamp >= min_timestamp) {
				/* проверяем доступность данных за требуемую дату */
				if(!check_timestamp(timestamp)) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}

				/* загружаем данные за торговый день */
				std::vector<Deal> temp;
				err = read_deals<STORAGE_TYPE>(temp, timestamp);
				if(err != xquotes_common::OK) {
					timestamp -= xtime::SECONDS_IN_DAY;
					continue;
				}

				/* обрабатываем данные */
				size_t temp_index = 0;
				while(temp_index < temp.size()) {
					const xtime::timestamp_t last_timestamp =
						temp[temp_index].timestamp + temp[temp_index].duration;
					if(last_timestamp > protection_timestamp) {
						temp.erase(temp.begin() + temp_index);
						continue;
					}
					++temp_index;
				}
				if(temp.size() != 0 && callback != nullptr) callback(temp);

				/* добавлям данные в общий вектор */
				if(temp.size() > 0) list_deals.insert(list_deals.end(), temp.begin(), temp.end());
				if(list_deals.size() >= number_deals) break;

				timestamp -= xtime::SECONDS_IN_DAY;
			}
			if(list_deals.size() < number_deals) {
				list_deals.clear();
				return NO_DATA_ACCESS;
			}
			/* сортируем данные */
			std::sort(list_deals.begin(), list_deals.end(), [](const Deal &lhs, const Deal &rhs) {
                return lhs.timestamp < rhs.timestamp;
            });
			if(list_deals.size() != number_deals) {
				const size_t offset = list_deals.size() - number_deals;
				list_deals.erase(list_deals.begin(), list_deals.begin() + offset);
			}
			return OK;
		}

		/** \brief Получить сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок заданного размера.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param list_deals Массив сделок
		 * \param symbol_index Индекс символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_fixed_number_deals(
				std::vector<Deal> &list_deals,
				const uint32_t symbol_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			return get_fixed_number_deals(
				list_deals,
				number_deals,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* удаляем ненужные данные */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						if(symbol_index != temp[temp_index].symbol) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					if(temp.size() != 0 && callback != nullptr) callback(temp);
				});
		}

		/** \brief Получить последние сделки за указанное количество дней
         *
         * Данный метод загрузит вектор сделок заданного размера.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param list_deals Массив сделок
		 * \param symbol_name Имя символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int get_fixed_number_deals(
				std::vector<Deal> &list_deals,
				const std::string &symbol_name,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			return get_fixed_number_deals(
				list_deals,
				number_deals,
				stop_timestamp,
				[&](std::vector<Deal> &temp) {
					/* удаляем ненужные данные */
					size_t temp_index = 0;
					while(temp_index < temp.size()) {
						if(symbol_name != temp[temp_index].get_name()) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						const uint32_t minute_day = xtime::get_minute_day(temp[temp_index].timestamp);
						if(start_minute_day > minute_day || minute_day >= stop_minute_day) {
							temp.erase(temp.begin() + temp_index);
							continue;
						}
						++temp_index;
					}
					if(temp.size() != 0 && callback != nullptr) callback(temp);
				});
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора указанного количества сделок.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param winrate Винрейт
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_fixed_number(
				T &winrate,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_fixed_number_deals(list_deals, number_deals, stop_timestamp, callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора указанного количества сделок.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param winrate Винрейт
		 * \param symbol_index Индекс символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_fixed_number(
				T &winrate,
				const uint32_t symbol_index,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_fixed_number_deals(
				list_deals,
				symbol_index,
				start_minute_day,
				stop_minute_day,
				number_deals,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Получить винрейт за указанное количество дней
         *
         * Данный метод получит винрейт вектора указанного количества сделок.
		 * Также метод отсортирует сделки по времени и удалит те сделки, которые "подсматривают" в будущее (за указанную метку времени)
         * \param winrate Винрейт
		 * \param symbol_name Имя символа
		 * \param start_minute_day Начальная минута дня (включительно)
		 * \param stop_minute_day Конечная минута дня (не включительно)
		 * \param number_deals Количество сделок
         * \param stop_timestamp Конечная дата
		 * \param callback Функция для обратного вызова, можно использовать для дополнительной фильтрации сделок
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		template<class T>
		int get_winrate_fixed_number(
				T &winrate,
				const std::string &symbol_name,
				const uint32_t start_minute_day,
				const uint32_t stop_minute_day,
				const uint32_t number_deals,
				const xtime::timestamp_t stop_timestamp,
				std::function<void(std::vector<Deal> &deals)> callback = nullptr) {
			winrate = 0;
			std::vector<Deal> list_deals;
			int err = get_fixed_number_deals(
				list_deals,
				symbol_name,
				start_minute_day,
				stop_minute_day,
				number_deals,
				stop_timestamp,
				callback);
			if(err != OK) return err;
			easy_bo::SimplifedTester<uint32_t> tester;
			for(size_t i = 0; i < list_deals.size(); ++i) {
				if(list_deals[i].result == EASY_BO_WIN) tester.add_deal(easy_bo::EASY_BO_WIN);
				else tester.add_deal(easy_bo::EASY_BO_LOSS);
			}
			winrate = tester.get_winrate<T>();
			return OK;
		}

		/** \brief Добавить сделку
         *
         * Метод не пропускает повторяющиеся сделки
         * \param deal Сделка
		 * \return Вернет 0 в случае успеха, иначе см. код ошибок в xquotes_common.hpp
         */
		int add_deal(const Deal& deal) {
			if(date_timestamp != xtime::get_first_timestamp_day(deal.timestamp)) {
				/* если новый день, то пора загрузить предыдущие данные в хранилище
				 * но сначала их надо слить с теми данными, что уже есть в хранилище (если они есть)
 				 */
				if(!check_timestamp(date_timestamp)) {
					/* если данных еще нет в хранилище за указанную дату,
					 * тогда просто запишем то, что есть
					 * очистим массив сделок и запишем в него новую сделку
					 */
					if(list_write_deals.size() != 0) {
						sort_list_deals(list_write_deals); // посортируем массив преде записью
						int err = write_deals<STORAGE_TYPE>(list_write_deals, date_timestamp);
						if(err != xquotes_common::OK) return err;
						list_write_deals.clear();
					}
					list_write_deals.push_back(deal);
				} else {
					/* данные уже есть! Придется сначала прочитать старые данные */
					std::vector<Deal> temp;
					int err = read_deals<STORAGE_TYPE>(temp, date_timestamp);
					if(err != xquotes_common::OK) return err;
					/* добавим во временный массив не повторяющиеся сделки */
					if(temp.size() > 0) {
						for(size_t i = 0; i < list_write_deals.size(); ++i) {
							if(!check_repeat_deal(temp, list_write_deals[i])) {
								temp.push_back(list_write_deals[i]);
								/* ужасно не оптимальный код */
								sort_list_deals(temp);
							}
						}
					} else {
						temp = list_write_deals;
						if(temp.size() > 0) sort_list_deals(temp);
					}
					/* а теперь все обратно запишем */
					err = write_deals<STORAGE_TYPE>(temp, date_timestamp);
					if(err != xquotes_common::OK) return err;
					/* теперь очистим массив сделок и добавим новую сделку за новую дату */
					list_write_deals.clear();
					list_write_deals.push_back(deal);
				}
				date_timestamp = xtime::get_first_timestamp_day(deal.timestamp);
			} else {
				/* Если та же самая дата, просто добавим сделку, предварительно проверив на совпадение */
				if(check_repeat_deal(list_write_deals, deal)) return OK;
				list_write_deals.push_back(deal);
				/* хрен с ним, с идеальным кодом */
				sort_list_deals(list_write_deals);
			}
			return OK;
		}

        /** \brief Добавить сделку
         *
         * \param name Имя символа
         * \param symbol Индекс символа
         * \param group Группа сделок
         * \param direction Направление ставки, покупка или продажа опциона
         * \param result Результат опциона, победа или поражение
         * \param duration Продолжительность бинарного опциона в секундах
         * \param timestamp Метка времени начала бинарного опицона
         * \return Код ошибки
         */
		int add_deal(
            const std::string &name,
            const uint8_t symbol,
            const uint8_t group,
            const int8_t direction,
            const int8_t result,
            const uint32_t duration,
            xtime::timestamp_t timestamp) {
            Deal deal;
            deal.set_name(name);
            deal.symbol = symbol;
            deal.group = group;
            deal.direction = direction;
            deal.result = result;
            deal.duration = duration;
            deal.timestamp = timestamp;
            return add_deal(deal);
        }

        /** \brief Получить список уникальных символов
         *
         * Данный метод поможет определить список используемых в статистике сделок символов (валютных пар)
         * \return Список используемых в статистике сделок символов (валютных пар)
         */
        std::list<std::string> get_list_unique_symbols() {
            xtime::timestamp_t min_timestamp = 0, max_timestamp = 0;
            int err = get_min_max_timestamp(min_timestamp, max_timestamp);
            if(err != easy_bo::OK) return std::list<std::string>();
            std::list<std::string> symbol_list;
            xtime::for_days(min_timestamp, max_timestamp, [&](const xtime::timestamp_t &t){
                std::vector<Deal> list_deals;
                int err = get_deals(list_deals, t);
                if(err != easy_bo::OK) return;
                std::for_each(list_deals.begin(), list_deals.end(), [&](const Deal &d) {
                    symbol_list.push_back(d.get_name());
                });
            });
            symbol_list.sort();
            symbol_list.unique();
            return symbol_list;
        }

        /** \brief Получить количество символов
         *
         * Данный метод поможет определить количество используемых в статистике сделок символов (валютных пар)
         * \return Вернет количество используемых символов или 0, если ошибка
         */
        uint32_t get_number_unique_symbols() {
            return get_list_unique_symbols().size();
        }

		~DealsDataStoreTemplate() {
			save();
		}
	};

	typedef DealsDataStoreTemplate<> DealsDataStore;    /**< Хранилище сделок с использованием JSON */
	typedef DealsDataStoreTemplate<ArrayDealsStorage> FastDealsDataStore;   /**< Хранилище строк с использованием бинарных данных */
}

#endif // EASY_BO_STANDARD_TESTER_HPP_INCLUDED
