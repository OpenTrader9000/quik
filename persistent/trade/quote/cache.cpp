#include "cache.hpp"
//#include "serializer.hpp"
#include "writer.hpp"
#include <common/message/trade/quotes.hpp>
#include <unordered_map>

namespace persistent {
namespace trade {
namespace quote {

cache::cache(std::string const& folder)
    : sink_with_logging("trade")
    , storage_folder_(folder) {}

cache::~cache() {

    wait_sink();
    // write last data
    flush();

    assert(messages_.empty());
}

void cache::consume(std::vector<ptr_t>& messages) {
    for (auto& mes : messages) {
        consume(std::move(mes));
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
    } else
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
    }

    messages_.clear();
}

} // namespace trade
} // namespace trade
} // namespace persistent

