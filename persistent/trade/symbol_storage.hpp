#pragma once

#include "quote/bulk.hpp"
#include "trade/bulk.hpp"
#include "quote/order_book.hpp"

#include <functional>

namespace persistent {
namespace trade {

    enum load_mode {
        TRADE = 1,
        ALL = 2
    };

using trades_bulk_ptr_t = trade::bulk_ptr_t;
using quotes_bulk_ptr_t = quote::bulk_ptr_t;

struct day {
    trades_bulk_ptr_t   trades_;
    quotes_bulk_ptr_t   quotes_;

    uint64_t start_day_in_ms() const;
    uint64_t end_day_in_ms() const;

private:
    uint64_t    start_day_;
};

struct ohlcv {
    unibcd_t open_;
    unibcd_t high_;
    unibcd_t low_;
    unibcd_t close_;
    unsigned volume_;
    int      open_interest_diff_;
};

enum period {
    p1min  = 1,
    p5min  = 2,
    p10min = 3,
    p15min = 4,
    p30min = 5,
    p60min = 6,
    pday   = 7,
};

struct series {
    std::string         sec_code_;
    std::vector<ohlcv>  series_;
    uint64_t            start_timestamp_;
    int64_t             shift_;
    period              period_;
};

// all info about symbol stores in this class
struct symbol_storage {

    using walk_callback_ft = std::function<void(common::storage::trade const& trd, common::storage::quote const& quot)>;

    static constexpr uint64_t year2286 = 10000000000000;
    
    symbol_storage();
    ~symbol_storage();

    void load(std::string const& path2folder, std::string const& symbol, load_mode mode = load_mode::TRADE,
              uint64_t start_timestamp = 0, uint64_t end_timestamp = year2286);

    void concrete_data(uint64_t start, uint64_t end, walk_callback_ft callback);

    series extract(uint64_t start, uint64_t end);

    quote::order_book extract_order_book_by_timestamp(uint64_t timestamp);

    /*
    TODO: make extract for quotes. Something like this
    order_book_info_series extract_order_book(uint64_t start, uint64_t end, TransformCallback&& transformCallback);
    */

 private:
    
     std::vector<day>    data_;
    
};

} // namespace trade
} // namespace persistent