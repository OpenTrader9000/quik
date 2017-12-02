#pragma once

#include <common/message/base.hpp>
#include <common/message/ptr.hpp>
#include <common/thread/sink_with_logging.hpp>

#include <unordered_set>
#include <unordered_map>


namespace persistent {
namespace trade {
namespace quote {

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
     
    using messages_sequence_t = std::vector<common::message::ptr>;
    using messages_storage_t = std::unordered_map<std::string, messages_sequence_t>; // seq_code->seq

    messages_storage_t                  messages_;
    //std::unordered_set<std::string>     not_finalized_files_;

};



//template <typename It>
//inline void cache::push(It iterator, size_t count) {
//    sink_mt::push(it, count);
//}

} // namespace trade
} // namespace trade
} // namespace persistent