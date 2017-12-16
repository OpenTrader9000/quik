#include "cache.hpp"
//#include "serializer.hpp"
#include <common/message/trade/trade.hpp>
#include <unordered_map>
#include "writer.hpp"

#include <utils/time/time.hpp>
#include <utils/filesystem/filesystem.hpp>
#include <utils/log/log.hpp>
#include <iomanip>

namespace persistent {
namespace trade {
namespace trade {
cache::cache(std::string const& folder)
    : sink_with_logging("quote")
    , storage_folder_(folder) {}

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

void cache::consume(std::vector<ptr_t>& messages) {
    for (auto& mes : messages) {
        auto code = mes->code_;
        try {
            consume(std::move(mes));
        } catch (std::exception& ex) {
            logs::error("Error when handle message with code ", code, ". Error: ", ex.what());
        }
    }
}

void cache::consume(ptr_t&& message) {

    using namespace common::message;

    if (message.is_code(codes::TRADE)) {
        auto trade = message.cast<common::message::trade::trade>();
        trade_cache_key key(trade->sec_code_, trade->server_timestamp());

        messages_[key].push_back(std::move(message));
    } else if (message.is_code(codes::FLUSH)) {
        flush();
    }
    else if (message.is_code(codes::COMPRESS)) {
        compress();
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

        not_finalized_files_.insert(p);
    }
    
    messages_.clear();
}

void cache::compress() {
    auto current_time = utils::time::timestamp_in_ms();

    constexpr uint64_t moscow_ts_diff_in_ms = 3 * 60 * 60 * 1000;

    // compute time
    time_t    t = static_cast<time_t>(current_time / 1000);
    struct tm time;
    ::localtime_s(&time, &t);

    auto day = utils::build_string(time.tm_year + 1900, std::setw(2), std::setfill('0'),
        time.tm_mon + 1, std::setw(2), std::setfill('0'), time.tm_mday);


    auto not_finalized = not_finalized_files_;
    for (std::string const& path : not_finalized_files_) {

        if (!utils::fs::exists(path)) {
            not_finalized_files_.erase(path);
            continue;
        }

        auto file_name = utils::fs::filename(path.c_str());
        if (strncmp(file_name.c_str(), day.c_str(), 8) == 0)
            continue;

        writer w(path, "");
        w.compress();

        not_finalized_files_.erase(path);

    }
}

} // namespace trade
} // namespace trade
} // namespace persistent