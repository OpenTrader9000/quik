#pragma once

#include <common/numbers/bcd.hpp>
#include <common/storage/trade.hpp>
#include <stdint.h>

#include "quote/order_book.hpp"
#include <vector>

namespace persistent {
namespace trade {

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

struct ohlcv {
    unibcd_t open_;
    unibcd_t high_;
    unibcd_t low_;
    unibcd_t close_;
    unsigned volume_;
    unsigned start_open_interest_;
    unsigned end_open_interest_;
    uint64_t open_timestamp_;
    uint64_t close_timestamp_;
};

struct series {
    std::string        sec_code_;
    std::vector<ohlcv> series_;
    period             period_;
    uint64_t           shift_;

    uint64_t start() const;
    uint64_t end() const;
};

struct data_visitor {
    virtual void on_trade(common::storage::trade const&) = 0;
    virtual void on_order_book(quote::order_book const&) = 0;
};

} // namespace trade
} // namespace persistent