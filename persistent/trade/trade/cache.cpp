#include "cache.hpp"
#include "serializer.hpp"
#include <common/message/trade/trade.hpp>
#include <unordered_map>

namespace persistent {
namespace trade {
namespace trade {
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

    if (message.is_code(codes::TRADE)) {
        messages_.push_back(std::move(message));
    } else if (message.is_code(codes::FLUSH)) {
        flush();
    } else
        assert(false);
}


void cache::flush() {

    using trade_t = common::message::trade::trade;

    std::unordered_map<std::string, unsigned> count_map;

    uint64_t min_ts    = std::numeric_limits<uint64_t>::max();
    uint64_t max_ts    = 0;
    uint64_t day_in_ms = 24 * 60 * 60 * 1000;
    uint64_t moscow_ts = 3 * 60 * 60 * 1000;

    // compute basic parameters
    for (auto& m : messages_) {
        trade_t* trd = m.cast<trade_t>();
        count_map[trd->sec_code_]++;
        min_ts = std::min(min_ts, trd->server_timestamp());
        max_ts = std::max(max_ts, trd->server_timestamp());
    }

    // walk throught all days for all messages
    // The most possible scenario that all messages are fit in one day.
    auto start_day_ts = min_ts - min_ts % day_in_ms - moscow_ts;
    while (start_day_ts < max_ts) {
        // walk every sec_code
        for (auto& pair : count_map) {
            serializer ser(pair.first, storage_folder_, start_day_ts, start_day_ts + day_in_ms);
            ser.serialize(messages_, pair.second );
        }
        start_day_ts += day_in_ms;
    }

    auto have_not_handled_messages = [this]() {
        for (auto& mes : messages_) {
            if (mes)
                return true;
        }
        return false;
    };
    assert(!have_not_handled_messages());

    messages_.clear();
}

} // namespace trade
} // namespace trade
} // namespace persistent