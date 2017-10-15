#include "order_book_diff.hpp"

namespace robot {
namespace thread {
namespace lua {
namespace details {
	 
order_book& order_book_diff::current() {
	if (selector_) 
		return ob1_;
	return ob2_;
}

order_book& order_book_diff::old() {
	if (selector_)
		return ob2_;
	return ob1_;
}


template <typename DiffCallback>
void diff(std::vector<order_book_element> const& newarray,
          std::vector<order_book_element> const& oldarray,
          DiffCallback&&                         diff_callback_f) {

    using quote_t = common::storage::quote;

    auto new_it     = newarray.begin();
    auto old_it     = oldarray.begin();
    auto new_end_it = newarray.end();
    auto old_end_it = oldarray.end();

    while (new_it != new_end_it || old_it != old_end_it) {

        bool new_reach_end = (new_it == new_end_it);
        bool old_reach_end = (old_it == old_end_it);

        // if has elements for a single one sequence
        if (new_reach_end || old_reach_end) {
			quote_t diff{};
            if (new_reach_end) {
                diff.price_         = old_it->price_;
                diff.quantity_diff_ = -old_it->quantity_;
                ++old_it;
            } else { // old_reach_end
                diff.price_         = new_it->price_;
                diff.quantity_diff_ = new_it->quantity_;
                ++new_it;
            }
            diff_callback_f(diff);
            continue;
        }

        if (new_it->price_ == old_it->price_) {
            // the most possible variant is this values are equal
            if (new_it->quantity_ != old_it->quantity_) {
				quote_t diff{};
                diff.price_         = new_it->price_;
                diff.quantity_diff_ = new_it->quantity_ - old_it->quantity_;
                diff_callback_f(diff);
            }

            ++new_it;
            ++old_it;
            continue;
        }

        if (new_it->price_ > old_it->price_) {
			quote_t diff{};
            diff.price_         = old_it->price_;
            diff.quantity_diff_ = -old_it->quantity_;
			diff_callback_f(diff);

            ++old_it;
			continue;
        }

        if (new_it->price_ < old_it->price_) {
			quote_t diff{};
            diff.price_         = new_it->price_;
            diff.quantity_diff_ = new_it->quantity_;
			diff_callback_f(diff);

            ++new_it;
			continue;
        }
    }
}

order_book_diff::order_book_diff(int fractional_size)
: ob1_(fractional_size)
, ob2_(fractional_size)
, selector_(true) {}

order_book_diff::~order_book_diff() {}

order_book_diff::result_t order_book_diff::mkdiff(sol::table ob) {

	selector_ = !selector_;

	// new current
	auto& new_ob = current();

	new_ob.extract(ob);

	return mkdiff_impl();
}


order_book_diff::result_t order_book_diff::mkdiff_impl() {
    result_t result = common::message::make < common::message::trade::quotes>();

	auto& new_ob = current();
	auto& old_ob = old();

	diff(new_ob.bid_, old_ob.bid_, [&](quote_t& res) {
		res.set_bid();
		result->quotes_.push_back(res);
	});

	diff(new_ob.offer_, old_ob.offer_, [&](quote_t& res) {
		res.set_offer();
		result->quotes_.push_back(res);
	});

	return result;
}

order_book_diff::result_t order_book_diff::mkdiff_last() {
    return mkdiff_impl();
}

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot
