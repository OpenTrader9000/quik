#pragma once

#include <common/message/base.hpp>
#include <common/message/ptr.hpp>
#include <common/thread/sink_with_logging.hpp>

#include <unordered_set>
#include <unordered_map>

namespace persistent {
namespace trade {
namespace trade {

struct cache : public common::thread::sink_with_logging_mt_t {
    
    cache(std::string const& folder);
    ~cache();

    //void push(ptr_t&& src);

    //template <typename It>
    //void push(It iterator, size_t count);

    void         consume(ptr_t&& message);
    virtual void consume(std::vector<ptr_t>& messages) override;

 private:
    
    std::string storage_folder_;

    void flush();
    void compress();

    // key must be a [sec_code, date] because trades are inpredictable
    struct trade_cache_key {
        std::string sec_code_;
        uint64_t    day_start_;

        trade_cache_key(std::string const& sec_code, uint64_t time);

        bool operator==(trade_cache_key const& other) const;
        
    };

    struct trade_cache_key_hash {
        size_t operator()(const trade_cache_key &k) const;
    };

    using messages_t = std::vector<common::message::ptr>;
    using message_storage_t = std::unordered_map<trade_cache_key, messages_t, trade_cache_key_hash>;

    message_storage_t                   messages_;

    std::unordered_set<std::string>     not_finalized_files_;

};



//template <typename It>
//inline void cache::push(It iterator, size_t count) {
//    sink_mt::push(it, count);
//}

} // namespace trade
} // namespace trade
} // namespace persistent