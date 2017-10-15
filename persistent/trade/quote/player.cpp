#include "player.hpp"

#include <common/container/small_vector.hpp>

namespace persistent {
namespace trade {
namespace quote {
player::player(std::vector<quote_t> const& quotes) : quotes_(quotes){}
player::player(std::vector<quote_t>&& quotes)
: quotes_(std::move(quotes)) {}
player::~player() {}

order_book const& player::book() const {
	return order_book_;
}
order_book const& player::next() {

    enum fsm_state { UPDATE_BID = 1, UPDATE_OFFER = 2 };

	fsm_state state = UPDATE_BID;

    if (quotes_index_ != quotes_.size()) {
		last_ts_ = quotes_[quotes_index_].machine_timestamp_;
	}

	auto& bid = order_book_.bid_;
	auto& offer = order_book_.offer_;

    auto bid_it   = bid.begin();
    auto offer_it = offer.begin();

	common::container::small_vector<quote_t, 16> bid_diff;
	common::container::small_vector<quote_t, 16> offer_diff;

    // walk throught all changes in this timestamp
    while (quotes_index_ != quotes_.size() && last_ts_ == quotes_[quotes_index_].machine_timestamp_) {
		auto& quote = quotes_[quotes_index_++];
		if (quote.is_bid()) {
			bid_diff.push_back(quote);
		}
		else {
			offer_diff.push_back(quote);
		}
    }

	// update bids loop
	for (auto& q : bid_diff) {

		if (bid_it == bid.end() || bid_it->price_ > q.price_)
			bid_it = bid.begin();
		
		while (bid_it != bid.end() && bid_it->price_ < q.price_)
			++bid_it;
		
		if (bid_it != bid.end() && bid_it->price_ == q.price_) {
			bid_it->quantity_ += q.quantity_diff_;
			if (bid_it->quantity_ == 0)
				bid_it = bid.erase(bid_it);
		}
		else {
			assert(q.quantity_diff_ > 0);
			order_book_element element;
            element.price_    = q.price_;
            element.quantity_ = q.quantity_diff_;
			bid_it = order_book_.bid_.insert(bid_it, element);
		}
	}

	for (auto& q : offer_diff) {

		if (offer_it == offer.end() || offer_it->price_ > q.price_)
			offer_it = offer.begin();

		while (offer_it != offer.end() && offer_it->price_ < q.price_)
			++offer_it;

		if (offer_it != offer.end() && offer_it->price_ == q.price_) {
			offer_it->quantity_ += q.quantity_diff_;
			if (offer_it->quantity_ == 0)
				offer_it = offer.erase(offer_it);
		}
		else {
			assert(q.quantity_diff_ > 0);
			order_book_element element;
			element.price_ = q.price_;
			element.quantity_ = q.quantity_diff_;
			offer_it = order_book_.offer_.insert(offer_it, element);
		}
	}

	return order_book_;
}

order_book const& player::to_ms(uint64_t ts) {
	while (last_ts_ < ts)
		next();
	return order_book_;
}

uint64_t player::timestamp() const {
    return last_ts_;
}

bool player::is_end() const {
    return quotes_index_ == quotes_.size();
}

} // namespace quote
} // namespace trade
} // namespace persistent