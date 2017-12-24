#pragma once

#include "quote/bulk.hpp"
#include "trade/bulk.hpp"
#include "quote/order_book.hpp"
#include "quote/order_book_constructor.hpp"

#include "data.hpp"

#include <functional>

namespace persistent {
namespace trade {

enum load_mode {
    TRADE = 1,
    ALL   = 2,
};

using trades_bulk_ptr_t = trade::bulk_ptr_t;
using quotes_bulk_ptr_t = quote::bulk_ptr_t;
using ob_constructor_ptr_t  = std::shared_ptr<quote::order_book_constructor>;

struct day {
    trades_bulk_ptr_t    trades_;
    ob_constructor_ptr_t quotes_;

    uint64_t day_start_in_ms() const;
    uint64_t day_end_in_ms() const;

    enum {
        HAS_TRADES = 1,
        HAS_QUOTES = 2,
    };

    unsigned flags_;
    uint64_t day_start_;
};


// all info about symbol stores in this class
struct symbol_storage {

    static constexpr uint64_t year2286 = 10000000000000;
    
    symbol_storage(std::string const& path2folder, std::string const& symbol);
    symbol_storage(symbol_storage&&) = default;
    ~symbol_storage();

    symbol_storage& operator=(symbol_storage&&) = default;

    // loads concrete days for concrete purposes
    void load_trades(day& d);
    void load_quotes(day& d);
    
    uint64_t start_timestamp() const;
    uint64_t end_timestamp() const;

    void concrete_data(uint64_t start, uint64_t end, data_visitor* callback) const;

    series extract(uint64_t start, uint64_t end, period per, int64_t shift);

    // here might be return const reference but have to have thrown an exception in case when timestamp doesn't found
    quote::order_book_state extract_order_book_by_timestamp(uint64_t timestamp);

    /*
    TODO: make extract for quotes. Something like this
    order_book_info_series extract_order_book(uint64_t start, uint64_t end, TransformCallback&& transformCallback);
    */

 private:
    // function doesn't create structure for days. It just scan all possible days in folder
    void scan_days();

    std::string path_to_folder_;
    std::string sec_code_;

    std::vector<day> data_;
    
};

} // namespace trade
} // namespace persistent