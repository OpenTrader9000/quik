#pragma once

#include <common/storage/quote.hpp>
#include <common/container/small_vector.hpp>

#include <vector>
#include "order_book.hpp"
#include "bulk.hpp"

namespace persistent {
namespace trade {
namespace quote {

struct order_book_shaper_state {
    order_book order_book_;
    bulk_state bulk_state_;

    uint64_t timestamp() const {
        return bulk_state_.quote_.machine_timestamp_;
    }
};

struct order_book_constructor {

	using quote_t = common::storage::quote;
    using tmp_vec_t = common::container::small_vector<quote_t, 16>;

    order_book_constructor(bulk_ptr_t data);
	~order_book_constructor();

    // functions change and return current state
	order_book_shaper_state const& book() const;
    order_book_shaper_state const& next_ts();
	order_book_shaper_state const& to_ts(uint64_t ts);

	bool is_end() const;  

    void make_index();

    void clear();

 private:
    void init();
    void apply_diff_to_current_order_book(tmp_vec_t const& bids_diff, tmp_vec_t const& offers_diff);

    bulk_ptr_t                           data_;
    std::vector<order_book_shaper_state> cache_;

    // last generated order book
    order_book_shaper_state current_state_;


    //std::vector<quote_t> quotes_;

 //   std::size_t          quotes_index_ = 0;
	//uint64_t			 last_ts_ = 0;
};
} // namespace quote
} // namespace trade
} // namespace persistent