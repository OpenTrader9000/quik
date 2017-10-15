#include "order_book.hpp"
#include <common/numbers/bcd.hpp>
#include <common/numbers/integer.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

void add_element(std::vector<order_book_element>& ob, sol::table const& tab, int fractional_size) {
    ob.emplace_back();
    auto& last = ob.back();

    std::string price    = tab["price"];
    std::string quantity = tab["quantity"];

    last.price_    = ::parse_bcd(price.c_str(), fractional_size);
    last.quantity_ = common::numbers::parse_int(quantity.c_str());
}

order_book::order_book(int fractional_size)
: fractional_size_(fractional_size) {}

order_book::~order_book()
{
}

void order_book::extract(sol::table tab) {
	sol::table bid = tab["bid"];
	sol::table offer = tab["offer"];
	
	unsigned bid_count = tab["bid_count"];
	unsigned offer_count = tab["offer_count"];

	bid_.clear();
	offer_.clear();

	if (bid_count != 0) {
		bid.for_each([&](sol::object const&, sol::table const& bid) {
			add_element(bid_, bid, fractional_size_);
		});
	}

	if (offer_count != 0) {
		offer.for_each([&](sol::object const&, sol::table const& offer) {
			add_element(offer_, offer, fractional_size_);
		});
	}

}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot