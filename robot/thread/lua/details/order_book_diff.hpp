#pragma once

#include "order_book.hpp"

#include <common/message/ptr.hpp>
#include <common/message/trade/quotes.hpp>
#include <sol/sol.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

struct order_book_diff {

    using result_t = common::message::ptr_concrete<common::message::trade::quotes>;
    using quote_t  = common::storage::quote;


    order_book_diff(int fractional_size = 0);
    ~order_book_diff();

    result_t mkdiff(sol::table ob);
    result_t mkdiff_last();

    order_book& current();
    order_book& old();

 private:
    result_t mkdiff_impl();

    order_book ob1_;
    order_book ob2_;

    bool selector_; // true - ob1, false - ob2
};

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot
