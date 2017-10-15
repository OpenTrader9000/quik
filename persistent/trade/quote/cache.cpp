#include "cache.hpp"
#include "serializer.hpp"
#include <common/message/trade/quotes.hpp>
#include <unordered_map>

namespace persistent {
namespace trade {
namespace quote {

cache::cache(std::string const& folder)
: storage_folder_(folder) {}

cache::~cache() {

    wait_sink();
    // write last data
    flush();

    assert(messages_.empty());
}

void cache::consume(ptr_t&& message) {

    using namespace common::message;

    if (message.is_code(codes::QUOTES)) {
        messages_.push_back(std::move(message));
    } else if (message.is_code(codes::FLUSH)) {
        flush();
    } else
        assert(false);
}


void cache::flush() {

    using quotes_t = common::message::trade::quotes;

    std::unordered_map<std::string, unsigned> count_map;

    uint64_t min_ts    = std::numeric_limits<uint64_t>::max();
    uint64_t max_ts    = 0;
    uint64_t day_in_ms = 24 * 60 * 60 * 1000;
    uint64_t moscow_ts = 3 * 60 * 60 * 1000;

    // compute basic parameters
    for (auto& m : messages_) {
        quotes_t* trd = m.cast<quotes_t>();
        count_map[trd->sec_code_]++;

		assert(!trd->quotes_.empty());
		min_ts = std::min(min_ts, trd->quotes_.front().machine_timestamp_);
    }

    // walk throught all days for all messages
    // The most possible scenario that all messages are fit in one day.
    auto start_day_ts = min_ts - min_ts % day_in_ms - moscow_ts;
    for (auto& pair : count_map) {
        serializer ser(pair.first, storage_folder_, start_day_ts);
        ser.serialize(messages_, pair.second );
    }

    messages_.clear();
}

} // namespace trade
} // namespace trade
} // namespace persistent