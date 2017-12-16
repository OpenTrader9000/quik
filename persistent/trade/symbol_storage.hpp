#pragma once

#include "quote/bulk.hpp"
#include "trade/bulk.hpp"
#include "quote/order_book.hpp"
#include "quote/order_book_constructor.hpp"

#include <functional>

namespace persistent {
namespace trade {

    enum load_mode {
        TRADE = 1,
        ALL = 2
    };

using trades_bulk_ptr_t = trade::bulk_ptr_t;
using quotes_bulk_ptr_t = quote::bulk_ptr_t;
using ob_constructor_ptr_t  = std::shared_ptr<quote::order_book_constructor>;

struct day {
    trades_bulk_ptr_t    trades_;
    ob_constructor_ptr_t quotes_;

    uint64_t start_day_in_ms() const;
    uint64_t end_day_in_ms() const;

//private:
    uint64_t    start_day_;
};

struct ohlcv {
    unibcd_t open_;
    unibcd_t high_;
    unibcd_t low_;
    unibcd_t close_;
    unsigned volume_;
    unsigned start_open_interest_;
    unsigned end_open_interest_;
    uint64_t open_timestamp_;

};


enum period {
    p1min  = 1,
    p5min  = 2,
    p10min = 3,
    p15min = 4,
    p30min = 5,
    p60min = 6,
    pday   = 7,
    pweek  = 8,
};

uint64_t period_in_ms(period p);

struct series {
    std::string         sec_code_;
    std::vector<ohlcv>  series_;
    period              period_;
    uint64_t            shift_;

    uint64_t start() const;
    uint64_t end() const;
};

struct data_visitor {
    virtual void on_trade(common::storage::trade const&) = 0;
    virtual void on_order_book(quote::order_book const&) = 0;
};

// all info about symbol stores in this class
struct symbol_storage {

    static constexpr uint64_t year2286 = 10000000000000;
    
    symbol_storage();
    ~symbol_storage();

    bool load(std::string const& path2folder, std::string const& symbol, load_mode mode = load_mode::TRADE,
              uint64_t start_timestamp = 0, uint64_t end_timestamp = year2286);

    void concrete_data(uint64_t start, uint64_t end, data_visitor* callback) const;

    series extract(uint64_t start, uint64_t end, period per, int64_t shift) const;

    // here might be return const reference but have to have thrown an exception in case when timestamp doesn't found
    quote::order_book_state extract_order_book_by_timestamp(uint64_t timestamp) const;

    /*
    TODO: make extract for quotes. Something like this
    order_book_info_series extract_order_book(uint64_t start, uint64_t end, TransformCallback&& transformCallback);
    */

 private:
    
     std::vector<day>    data_;
     std::string         sec_code_;
    
};

} // namespace trade
} // namespace persistent