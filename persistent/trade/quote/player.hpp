#pragma once

#include <common/storage/quote.hpp>
#include <vector>
#include "order_book.hpp"

namespace persistent {
namespace trade {
namespace quote {
struct player {

	using quote_t = common::storage::quote;

	player(std::vector<quote_t> const& quotes);
	player(std::vector<quote_t>&& quotes);
	~player();

	order_book const& book() const;
    order_book const& next();
	order_book const& to_ms(uint64_t ts);

	uint64_t timestamp() const;

	bool is_end() const;

 private:
    order_book           order_book_;
    std::vector<quote_t> quotes_;
    std::size_t          quotes_index_ = 0;
	uint64_t			 last_ts_ = 0;
};
} // namespace quote
} // namespace trade
} // namespace persistent