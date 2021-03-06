#pragma once

#include <common/message/message.hpp>
#include <thread>
#include <common/defines.hpp>

namespace common {
namespace thread {

template <typename QueueType>
struct sink {

    using ptr_t = ::common::message::ptr;
    using bulk_t = std::vector<ptr_t>;

    sink();
    virtual ~sink();

    // cannot run class before construction of it
    void start_sink();

    // main loop for a handling messages
    void loop();

    // wait sink for a syncronization purposes
    void wait_sink();

    // push new message in async queue
    void push(::common::message::ptr&&);

    // push message with copy
    void push(::common::message::ptr& mes);

    // works just with multithread queue
    template <typename It>
    void push_bulk(It first, size_t count) {
        queue_.enqueue_bulk(first, count);
    }

    // client messaging
    //virtual void consume(::common::message::ptr&&) = 0;
    
    // consumer thread owns all messages
    virtual void consume(bulk_t&) = 0;

    // event messaging
    virtual void on_start_thread()                  = 0;
    virtual void on_end_thread()                    = 0;
    virtual void on_handle_messages()               = 0;
    virtual void on_error(std::string const& error) = 0;

    bool sink_stopped_ = false;
    std::thread thread_;
    QueueType queue_;
};

using sink_st_t = sink<reader_writer_queue_t>;
using sink_mt_t = sink<multithread_queue_t>;   


} // namespace thread
} // namespace common
