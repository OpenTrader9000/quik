#include "symbol_storage.hpp"
#include <utils/filesystem/filesystem.hpp>
#include "quote/reader.hpp"
#include "trade/reader.hpp"

#include <time.h>
#include <iomanip>

namespace persistent {
namespace trade {

symbol_storage::symbol_storage()
     {}

symbol_storage::~symbol_storage() {}

// helper function for computation year
std::string ms_to_YMD(uint64_t ms_data) {

    struct tm t;
    time_t time = static_cast<time_t>(ms_data / 1000);

    gmtime_s(&t, &time);

    return utils::build_string(1900 + t.tm_year, std::setw(2), std::setfill('0'), t.tm_mon + 1,
        std::setw(2), std::setfill('0'), t.tm_mday);
}

uint64_t YMD_to_ms(std::string const& str_date) {
    
    // str_date is not a big number 8-9 symbols long
    auto date = std::strtol(str_date.c_str(), nullptr, 10);

    struct tm t {};

    t.tm_mday = (date % 100);
    date /= 100;
    t.tm_mon = (date % 100) - 1;
    t.tm_year = (date / 100) - 1900;

    // time in ms
    return static_cast<uint64_t>(std::mktime(&t))*1000;
}

void symbol_storage::load(std::string const& path2folder, std::string const& symbol, load_mode mode,
                          uint64_t start_timestamp, uint64_t end_timestamp) {

    // every time symbol is subfolder of the year
    auto symbol_folder_path = utils::build_string(path2folder, '/', symbol);
    if (!utils::fs::exists(symbol_folder_path)) {
        return;
    }

    auto trades = utils::fs::files_in_directory(utils::build_string(symbol_folder_path, "/*.trd"));
    auto quotes = utils::fs::files_in_directory(utils::build_string(symbol_folder_path, "/*.ob"));

    bool load_quotes = (mode == load_mode::ALL);
    auto q_it = quotes.begin();
    auto q_end = quotes.end();

    std::string low_ts_str  = ms_to_YMD(start_timestamp);
    std::string high_ts_str = ms_to_YMD(end_timestamp);

    auto compare = [](std::string const& lhs, std::string const& rhs) {
        return strncmp(lhs.c_str(), rhs.c_str(), 8);
    };

    for (std::string const& trade_path : trades) {

        // check bounds
        if (compare(trade_path, low_ts_str) > 0 || compare(trade_path, high_ts_str) < 0) {
            continue;
        }

        day d;

        // load trades
        d.trades_ = trade::reader(trade_path).bulk();

        // find compatible quote
        while (load_quotes && (compare(trade_path, *q_it) > 0) && q_it != q_end) {
            ++q_it;
        }

        // load quote
        if (load_quotes && (q_it != q_end) && (compare(trade_path, *q_it) == 0)) {
            d.quotes_ = quote::reader(*q_it).bulk();
        }

        data_.push_back(d);
    }
}

void symbol_storage::concrete_data(uint64_t start, uint64_t end, walk_callback_ft callback) {

    // period should be fit in a day
    bool wrong_ts = end < start || (end - start) > 60 * 60 * 1000;
    assert(!wrong_ts);
    if (wrong_ts) {
        return;
    }

    /*TODO: Implement*/
}

uint64_t day::start_day_in_ms() const {
    return start_day_ ;
}

uint64_t day::end_day_in_ms() const {
    return start_day_ + 24 * 60 * 60 * 1000;
}

} // namespace trade
} // namespace persistent