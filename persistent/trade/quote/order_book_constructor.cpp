#include "order_book_constructor.hpp"

namespace persistent {
namespace trade {
namespace quote {

order_book_constructor::order_book_constructor(bulk_ptr_t data)
    : data_(data) {
    init();
}

order_book_constructor::~order_book_constructor()
{
}

order_book_state const & order_book_constructor::book() const
{
    return current_state_;
}

order_book_state const& order_book_constructor::next_ts() {

    if (is_end())
        return current_state_;

    auto& blk_state = current_state_.bulk_state_;
    auto  last_ts   = current_state_.timestamp();

    //assert(last_ts != 0);

    tmp_vec_t bid_diff;
    tmp_vec_t offer_diff;

    // extract data from bulk
    current_state_.bulk_state_ = data_->walk(blk_state, [&](common::storage::quote const& q) mutable {
        // setup bid and quote
        if (q.is_bid()) {
            bid_diff.push_back(q);
        } else {
            offer_diff.push_back(q);
        }
        return last_ts == q.machine_timestamp_;
    });

    apply_diff_to_current_order_book(bid_diff, offer_diff);

    return current_state_;
}

order_book_state const& order_book_constructor::to_ts(uint64_t ts) {
    
    // helper for arounding cases when low value is higher high value
    auto diff = [](uint64_t higher_ts, uint64_t lower_ts) {
        if (lower_ts > higher_ts)
            return std::numeric_limits<uint64_t>::max();
        return higher_ts - lower_ts;
    };

    // find the nearest order book for this timestamp
    order_book_state* nearest_element = &current_state_;
    uint64_t min_diff = diff(ts, current_state_.timestamp());
    for (auto& stat : cache_) {
        auto cur_diff = diff(ts, stat.timestamp());
        if (cur_diff < min_diff){
            min_diff = cur_diff;
            nearest_element = &stat;
        }
    }

    if (nearest_element != &current_state_) {
        current_state_ = *nearest_element;
    }

    tmp_vec_t bid_diff;
    tmp_vec_t offer_diff;

    current_state_.bulk_state_ = data_->walk(current_state_.bulk_state_, [&](common::storage::quote const& q) mutable {
        // setup bid and quote
        if (q.is_bid()) {
            bid_diff.push_back(q);
        }
        else {
            offer_diff.push_back(q);
        }
        return q.machine_timestamp_ < ts;
    });

    apply_diff_to_current_order_book(bid_diff, offer_diff);
    
    return current_state_;
}

bool order_book_constructor::is_end() const
{
    return current_state_.bulk_state_.is_end_reached();;
}

void order_book_constructor::make_index(){
    
    constexpr uint64_t half_an_hour_in_ms = 1800 * 1000;
    
    if (!cache_.empty()){
        return;
    }

    init();

    cache_.push_back(current_state_);
    
    uint64_t ts = current_state_.timestamp();
    while (!is_end()){
        ts += half_an_hour_in_ms;
        cache_.push_back(to_ts(ts));
    }
}

void order_book_constructor::clear(){
    init();
}

void order_book_constructor::init()
{
    current_state_ = order_book_state{};

    // read all order book
    next_ts(); // read first element from buffer
    next_ts(); 
}

void order_book_constructor::apply_diff_to_current_order_book(tmp_vec_t const& bids_diff,
                                                              tmp_vec_t const& offers_diff) {

    auto& bid   = current_state_.order_book_.bid_;
    auto& offer = current_state_.order_book_.offer_;

    // apply diff array to bids or offers
    auto apply_updates = [](std::vector<order_book_element>& updatable, tmp_vec_t const& ob_diff) {

        auto it  = updatable.begin();
        auto end = updatable.end();

        for (auto& q : ob_diff) {

            // current quote was slipped through
            if (it == updatable.end() || it->price_ > q.price_) {
                it = updatable.begin();
            }

            // rewind array to proper place
            while (it != updatable.end() && it->price_ < q.price_) {
                ++it;
            }

            // proper price was found in order book
            if (it != updatable.end() && it->price_ == q.price_) {
                it->quantity_ += q.quantity_diff_;
                if (it->quantity_ == 0) {
                    it = updatable.erase(it);
                }
            } else {

                // proper price was not found in order book
                // amount of elements for this price must be greater than zero
                assert(q.quantity_diff_ > 0);

                order_book_element element;
                element.price_    = q.price_;
                element.quantity_ = q.quantity_diff_;
                it                = updatable.insert(it, element);
            }
        }
    };

    apply_updates(bid, bids_diff);
    apply_updates(offer, offers_diff);
}

} // namespace quote
} // namespace trade
} // namespace persistent