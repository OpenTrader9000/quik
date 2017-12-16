#include "cache.hpp"
//#include "serializer.hpp"
#include "writer.hpp"
#include <common/message/trade/quotes.hpp>
#include <unordered_map>
#include <utils/time/time.hpp>
#include <utils/filesystem/filesystem.hpp>
#include <utils/log/log.hpp>
#include <iomanip>

namespace persistent {
namespace trade {
namespace quote {

cache::cache(std::string const& folder)
    : sink_with_logging("quote")
    , storage_folder_(folder) {}

cache::~cache() {

    wait_sink();
    // write last data
    flush();

    assert(messages_.empty());
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
    using namespace common::message::trade;


    if (message.is_code(codes::QUOTES)) {
        quotes* q = message.cast<quotes>();
        messages_[q->sec_code_].push_back(std::move(message));
    } else if (message.is_code(codes::FLUSH)) {
        flush();
    }
    else if (message.is_code(codes::COMPRESS)) {
        compress();
    }
    else
        assert(false);
}


void cache::flush() {

    using quotes_t = common::message::trade::quotes;

    // 
    for (auto& pair : messages_) {
        uint64_t timestamp = pair.second.front().cast<quotes_t>()->quotes_.front().machine_timestamp_;
        std::string p = path(pair.first, storage_folder_, timestamp);

        writer   ser(p, pair.first);
        ser.serialize(pair.second);

        not_finalized_files_.insert(p);
    }

    messages_.clear();
}

void cache::compress(){

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

        writer w(path,"");
        w.compress();

        not_finalized_files_.erase(path);
    }
}

} // namespace trade
} // namespace trade
} // namespace persistent

