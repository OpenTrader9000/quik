#pragma once

#include "sink.hpp"
#include <string>
#include <utils/log/log.hpp>

namespace common {
namespace thread {

template <typename QueueType>
struct sink_with_logging : public sink<QueueType> {

 public:
    sink_with_logging(std::string const& log_thread_name)
        : log_thread_name_(log_thread_name) {}
    ~sink_with_logging() {}

    virtual void on_start_thread() override { utils::log::set_thread_name(log_thread_name_); }
    virtual void on_end_thread() override {}
    virtual void on_handle_messages() override { utils::log::flush_thread_local_buffer(); }
    virtual void on_error(std::string const& error) override { logs::error(error); }

 private:
    std::string log_thread_name_;
};


using sink_with_logging_st_t = sink_with_logging<reader_writer_queue_t>;
using sink_with_logging_mt_t = sink_with_logging<multithread_queue_t>;

} // namespace thread
} // namespace common