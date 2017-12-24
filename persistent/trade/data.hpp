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
char const* period_str(period p);

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

struct series_description {
    period      period_;
    std::string sec_code_;
    uint64_t    shift_;

    struct equals {
        bool operator()(series_description const& lhs, series_description const& rhs) const {
            return (lhs.period_ == rhs.period_) && (lhs.sec_code_ == rhs.sec_code_) &&
                   (lhs.shift_ == rhs.shift_);
        }
    };

    struct hash {
        size_t operator()(series_description const& value) const {
            return std::hash<std::string>()(value.sec_code_) +
                   std::hash<int>()(static_cast<int>(value.period_)) + std::hash<uint64_t>()(value.shift_);
        }
    };

    static series_description make_empty() {
        series_description result{};

        // use probably the most unused period for an empty key
        result.period_ = period::pweek;

        return result;
    }
};


struct series {

    series_description description_; // or key
    std::vector<ohlcv> series_;

    uint64_t start() const;
    uint64_t end() const;
};

struct data_visitor {
    virtual void on_trade(common::storage::trade const&) = 0;
    virtual void on_order_book(quote::order_book const&) = 0;
};

} // namespace trade
} // namespace persistent