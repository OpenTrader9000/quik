#pragma once

#include <persistent/trade/data.hpp>
#include <common/container/array_view.hpp>

#include <memory>

using ohlc_t         = persistent::trade::ohlcv;
using ohlc_view_t    = common::container::array_view<ohlc_t const>;
using strings_view_t = common::container::array_view<std::string>;
using order_book_t   = persistent::trade::quote::order_book;
using period_t       = persistent::trade::period;

namespace view {
namespace model {

enum ds_flags {
    ticks      = 1,
    p1min      = 2,
    p5min      = 4,
    p10min     = 8,
    p15min     = 16,
    p30min     = 32,
    phour      = 64,
    pday       = 128,
    pweek      = 256,
    order_book = 512,
    all = ticks | p1min | p5min | p10min | p15min | p30min | phour | pday | pweek | order_book,
};

using period_t = persistent::trade::period;

// abstract class for creating ohlc and other type of sources
struct datasource {
 private:
    unsigned flags_;

 public:

     using series_key_t = persistent::trade::series_description;

    datasource(unsigned flags);
    virtual ~datasource();

    unsigned flags() const;

    bool has_period(period_t period) const;

    virtual ohlc_view_t  get_elements(series_key_t key, unsigned start_index, unsigned count)   = 0;
    virtual ohlc_view_t  get_elements_starts_with_index(series_key_t key, unsigned start_index) = 0;
    virtual order_book_t get_order_book(std::string const& sec_code, uint64_t ts)               = 0;
    virtual strings_view_t symbols()                                                            = 0;
    virtual size_t         count_of_elements(series_key_t key)                                  = 0;
};

using datasource_ptr_t = std::shared_ptr<datasource>;

inline datasource::series_key_t series_key(std::string const &sec_code, period_t period, uint64_t shift = 0) {
    datasource::series_key_t result;
    result.sec_code_ = sec_code;
    result.period_   = period;
    result.shift_    = shift;
    return result;
}

void make_persistent_datasource(std::string const& path2archive);
datasource_ptr_t get_datasource();

} // namespace model
} // namespace view