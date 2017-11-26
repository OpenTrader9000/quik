#include "cache.hpp"
//#include "serializer.hpp"
#include <common/message/trade/trade.hpp>
#include <unordered_map>
#include "writer.hpp"

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

cache::trade_cache_key::trade_cache_key(std::string const& sec_code, uint64_t time)
    : sec_code_(sec_code) {
    constexpr uint64_t day_in_ms = 24 * 60 * 60 * 1000;
    constexpr uint64_t moscow_ts = 3 * 60 * 60 * 1000;
    uint64_t local_time = (time - moscow_ts);
    day_start_ = local_time - local_time % day_in_ms;
}

bool cache::trade_cache_key::operator==(trade_cache_key const & other) const {
    return (sec_code_ == other.sec_code_) && (day_start_ == other.day_start_);
}

size_t cache::trade_cache_key_hash::operator()(const trade_cache_key & k) const {
    // all days in a single bucket
    return std::hash<std::string>()(k.sec_code_);
}


void cache::consume(ptr_t&& message) {

    using namespace common::message;

    if (message.is_code(codes::TRADE)) {
        auto trade = message.cast<common::message::trade::trade>();
        trade_cache_key key(trade->sec_code_, trade->server_timestamp());

        messages_[key].push_back(std::move(message));
    } else if (message.is_code(codes::FLUSH)) {
        flush();
    } else
        assert(false);
}


void cache::flush() {

    using trade_t = common::message::trade::trade;

    // compute basic parameters
    for (auto& m : messages_) {

        auto& key = m.first;
        //auto& messages = m.second;

        auto p = path(key.sec_code_, storage_folder_, key.day_start_);
        writer w(p, key.sec_code_);
        w.serialize(m.second);
    }
    
    messages_.clear();
}

} // namespace trade
} // namespace trade
} // namespace persistent