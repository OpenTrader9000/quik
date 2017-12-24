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

// functions helpers for parsing an interval
void on_interval_start(ohlcv& updatable, common::storage::trade const& trade);
void on_interval_update(ohlcv& updatable, common::storage::trade const& trade);
void on_interval_end(ohlcv& updatable, common::storage::trade const& trade);

symbol_storage::symbol_storage(std::string const& path2folder, std::string const& symbol)
    : path_to_folder_(path2folder)
    , sec_code_(symbol) {
    scan_days();
}

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

void symbol_storage::scan_days() {

    constexpr unsigned str_timestamp_size = 8;

    // every time symbol is subfolder of the year
    auto symbol_folder_path = utils::build_string(path_to_folder_, '/', sec_code_);
    if (!utils::fs::exists(symbol_folder_path)) {
        return;
    }
        
    auto trades = utils::fs::files_in_directory(utils::build_string(symbol_folder_path, "/*.trd"));
    auto quotes = utils::fs::files_in_directory(utils::build_string(symbol_folder_path, "/*.ob"));
    
    auto q_it = quotes.begin();
    auto q_end = quotes.end();
    
    auto compare = [str_timestamp_size](std::string const& lhs, std::string const& rhs) {
        return strncmp(lhs.c_str(), rhs.c_str(), str_timestamp_size);
    };
    
    for (std::string const& trade_path : trades) {
   
        day d{};
                    
        auto cur_str_ts = trade_path.substr(0, str_timestamp_size);
        d.day_start_= YMD_to_ms(cur_str_ts);
        d.flags_ |= day::HAS_TRADES;

        // find compatible quote
        while ((q_it != q_end) && (compare(trade_path, *q_it) > 0)) {
            ++q_it;
        }
    
        // load quote
        if ((q_it != q_end) && (compare(trade_path, *q_it) == 0)) {
            d.flags_ |= day::HAS_QUOTES;
        }
    
        data_.push_back(std::move(d));
    }
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
        if (end <= day.day_end_in_ms() && start>= day.day_start_in_ms()) {
            day.trades_->walk_period(start, end, [callback](common::storage::trade const& value) {
                callback->on_trade(value);
            });
        }
    }
}

series symbol_storage::extract(uint64_t start, uint64_t end, period per, int64_t shift) {

    // TODO: make indexed cache of series. Extract series from it here
    series result;


    result.description_.sec_code_ = sec_code_;
    result.description_.period_   = per;
    result.description_.shift_    = shift;

    // intersection
    uint64_t low_ts = std::max(start, data_.front().day_start_in_ms());
    uint64_t high_ts = std::min(end, data_.back().day_end_in_ms());


    if (data_.empty() || low_ts > high_ts) {
        return result;
    }

    result.series_.reserve((high_ts - low_ts) / period_in_ms(per));

    for (auto& day : data_) {
        
        // find out intersection of incoming range and day
        auto low  = std::max(start, day.day_start_in_ms());
        auto high = std::min(end, day.day_end_in_ms());
        if (low > high)
            continue;

        load_trades(day);

        ohlcv ohlc;
        day.trades_->walk_period_by_interval(low, high, shift, period_in_ms(per),
                                             [&](common::storage::trade const& t) {
                                                 on_interval_start(ohlc, t);
                                             },
                                             [&](common::storage::trade const& t) {
                                                 on_interval_update(ohlc, t);
                                             },
                                             [&](common::storage::trade const& t) {
                                                 on_interval_end(ohlc, t);
                                                 result.series_.push_back(ohlc);
                                             });
    }

    return result;
}

quote::order_book_state symbol_storage::extract_order_book_by_timestamp(uint64_t timestamp) {

    // moscow time to utc
    uint64_t utc_timestamp = timestamp - 3600 * 1000 * 3;

    for (auto& day : data_) {
        if (utc_timestamp <= day.day_end_in_ms() && utc_timestamp >= day.day_start_in_ms()) {
            load_quotes(day);
            return day.quotes_->to_ts(utc_timestamp);
        }
    }
    
    return quote::order_book_state();
}


void symbol_storage::load_trades(day& d) {

    constexpr uint64_t moscow_shift = 3 * 60 * 60 * 1000;

    if (d.trades_)
        return;

    
    auto trade_file_path =
        utils::build_string(path_to_folder_, "/", sec_code_, "/", ms_to_YMD(d.day_start_ + moscow_shift), ".trd");

    d.trades_ = trade::reader(trade_file_path).bulk();
}

void symbol_storage::load_quotes(day& d) {

    constexpr uint64_t moscow_shift = 3 * 60 * 60 * 1000;

    if (d.quotes_)
        return;

    auto quote_file_path =
        utils::build_string(path_to_folder_, "/", sec_code_, "/", ms_to_YMD(d.day_start_ + moscow_shift), ".ob");

    auto bulk = quote::reader(quote_file_path).bulk();
    d.quotes_ = std::make_shared<quote::order_book_constructor>(bulk);

    // make index in this place
    // TODO: store index in index files
    d.quotes_->make_index();
}

uint64_t symbol_storage::start_timestamp() const {
    if (data_.empty())
        return std::numeric_limits<uint64_t>::max();
    return data_.front().day_start_in_ms();
}

uint64_t symbol_storage::end_timestamp() const {
    if (data_.empty())
        return std::numeric_limits<uint64_t>::max();
    return data_.back().day_end_in_ms();
}

uint64_t day::day_start_in_ms() const {
    return day_start_ ;
}

uint64_t day::day_end_in_ms() const {
    return day_start_ + 24 * 60 * 60 * 1000;
}

void on_interval_start(ohlcv& updatable, common::storage::trade const& t) {
    updatable.open_               = t.price_;
    updatable.open_timestamp_     = t.server_timestamp_;
    updatable.volume_             = 0;
    updatable.low_                = t.price_;
    updatable.high_               = t.price_;
    updatable.start_open_interest_ = t.open_interest_;
    on_interval_update(updatable,t);
}

void on_interval_update(ohlcv& updatable, common::storage::trade const& t) {

    updatable.volume_ += t.quantity_;

    if (t.price_ < updatable.low_) {
        updatable.low_ = t.price_;
    }

    if (t.price_ > updatable.high_) {
        updatable.high_ = t.price_;
    }
}

void on_interval_end(ohlcv& updatable, common::storage::trade const& t) {
    updatable.end_open_interest_ = t.open_interest_;
    updatable.close_             = t.price_;
    updatable.close_timestamp_   = t.server_timestamp_;
}

} // namespace trade
} // namespace persistent