
#pragma once

#include "order_book_element.hpp"
#include <vector>
#include <sol/sol.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

struct order_book {
    std::vector<order_book_element> bid_;
    std::vector<order_book_element> offer_;
    int                             fractional_size_;
    uint64_t                        timestamp_;

    order_book(int fractional_size = 0);
	~order_book();

    bool empty() const { return bid_.empty() && offer_.empty(); }

    void extract(sol::table tab);
};

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot
