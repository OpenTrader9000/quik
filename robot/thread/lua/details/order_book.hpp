
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
	int fractional_size_;

	order_book(int fractional_size = 0);
	~order_book();

	void extract(sol::table tab);
};

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot