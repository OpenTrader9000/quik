#pragma once

#include <common/numbers/bcd.hpp>
#include <vector>
#include <list>

namespace persistent {
namespace trade {
namespace quote {
struct order_book_element {
    common::numbers::bcd price_;
    int                  quantity_;
};

struct order_book {
	std::vector<order_book_element>		bid_;
	std::vector<order_book_element>		offer_;
};

} // namespace quote
} // namespace trade
} // namespace persistent