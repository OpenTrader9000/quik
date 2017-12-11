#include "symbol_storage.hpp"
#include <utils/filesystem/filesystem.hpp>
#include "quote/reader.hpp"
#include "trade/reader.hpp"
#include "quote/order_book_constructor.hpp"

#include <time.h>
#include <iomanip>
#include <future>

namespace persistent {
namespace trade {

uint64_t period_in_ms(period p) {
    // choose time
    switch (p) {
    case period::p1min:
        return 60 * 1000;
    case period::p5min:
        return 300 * 1000;
    case period::p10min:
        return 600 * 1000;
    case period::p15min:
        return 900 * 1000;
    case period::p30min:
        return 1800 * 1000;
    case period::p60min:
        return 3600 * 1000;
    case period::pday:
        return 24 * 3600 * 1000;
    case period::pweek:
        return 7 * 24 * 7 * 1000;
    }
    return 0;
}

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

bool symbol_storage::load(std::string const& path2folder, std::string const& symbol, load_mode mode,
                          uint64_t start_timestamp, uint64_t end_timestamp) {

    // every time symbol is subfolder of the year
    auto symbol_folder_path = utils::build_string(path2folder, '/', symbol);
    if (!utils::fs::exists(symbol_folder_path)) {
        return false;
    }

    sec_code_ = symbol;

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

    std::vector<std::future<void>> tasks;
    
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
            d.quotes_ = std::make_unique<quote::order_book_constructor>(quote::reader(*q_it).bulk());
            tasks.push_back(std::async(std::launch::async, [&]() { d.quotes_->make_index(); }));
        }

        data_.push_back(std::move(d));
    }

    // wait all make_index tasks
    std::for_each(tasks.begin(), tasks.end(), std::mem_fn(&std::future<void>::wait));

    return true;
}

void symbol_storage::concrete_data(uint64_t start, uint64_t end, data_visitor* callback) const {

    constexpr uint64_t day_in_ts = 24 * 60 * 60 * 1000;

    uint64_t start_day_in_ms = (start / day_in_ts) * day_in_ts;
    uint64_t end_day_in_ms = start_day_in_ms + day_in_ts;

    // period should be fit in a day
    bool wrong_ts = end < start || (end - start) > day_in_ts || start < start_day_in_ms || end > end_day_in_ms;
    assert(!wrong_ts);
    if (wrong_ts) {
        return;
    }

    for (auto const& day : data_) {
        if (end <= day.end_day_in_ms() && start>= day.start_day_in_ms()) {
            day.trades_->walk_period(start, end, [callback](common::storage::trade const& value) {
                callback->on_trade(value);
            });
        }
    }
}

series symbol_storage::extract(uint64_t start, uint64_t end, period per, int64_t shift) const{

    series result;

    result.series_.reserve((end - start) / period_in_ms(per));
    result.sec_code_ = sec_code_;
    for (auto const& day : data_) {
        
        // find out intersection of incoming range and day
        auto low  = std::max(start, day.start_day_in_ms());
        auto high = std::min(end, day.end_day_in_ms());
        if (low > high)
            continue;

        ohlcv res{};

        day.trades_->walk_period(low + shift, high, [&](common::storage::trade const& t) {
            // TODO: Implement. Change logic in operators of bcd numbers for this
        });
    }

    return series();
}

quote::order_book_state symbol_storage::extract_order_book_by_timestamp(uint64_t timestamp) const {
    for (auto const& day : data_) {
        if (timestamp <= day.end_day_in_ms() && timestamp >= day.start_day_in_ms()) {
            return day.quotes_->to_ts(timestamp);
        }
    }
    
    return quote::order_book_state();
}

uint64_t day::start_day_in_ms() const {
    return start_day_ ;
}

uint64_t day::end_day_in_ms() const {
    return start_day_ + 24 * 60 * 60 * 1000;
}

uint64_t series::start() const {
    if (series_.empty())
        return 0;
    return series_.front().open_timestamp_;
}

uint64_t series::end() const {
    if (series_.empty())
        return 0;
    return series_.back().open_timestamp_ + period_in_ms(period_);
}

} // namespace trade
} // namespace persistent