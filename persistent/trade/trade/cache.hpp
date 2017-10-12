#pragma once

#include <common/message/base.hpp>
#include <common/message/ptr.hpp>
#include <common/thread/sink.hpp>


namespace persistent {
namespace trade {
namespace trade {

struct cache : public common::thread::sink_mt_t {
    
    cache(std::string const& folder);
    ~cache();

    //void push(ptr_t&& src);

    //template <typename It>
    //void push(It iterator, size_t count);

    virtual void consume(ptr_t&& message) override;

private:
    
    std::string storage_folder_;

    void flush();

    std::vector<common::message::ptr>   messages_;

};



//template <typename It>
//inline void cache::push(It iterator, size_t count) {
//    sink_mt::push(it, count);
//}

} // namespace trade
} // namespace trade
} // namespace persistent