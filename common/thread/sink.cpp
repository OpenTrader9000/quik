#include "sink.hpp"
#include "run.hpp"
#include <utility>

namespace common {
namespace thread {

template <typename QueueType>
sink<QueueType>::sink() {
}

template <typename QueueType>
sink<QueueType>::~sink() {
    assert(sink_stopped_);
}

template <typename QueueType>
void sink<QueueType>::start_sink() {
    thread_ = std::thread(&sink<QueueType>::loop, this);
}
template <typename QueueType>
void sink<QueueType>::wait_sink() {
    if (thread_.joinable())
        thread_.join();
    sink_stopped_ = true;
}


template <typename QueueType>
void sink<QueueType>::push(::common::message::ptr&& message) {
    queue_.enqueue(std::move(message));
}

template <typename QueueType>
void sink<QueueType>::push(::common::message::ptr& message) {
    queue_.enqueue(message);
}


template <>
void sink<reader_writer_queue_t>::loop() {

    bulk_t bulk;
    bulk.resize(1);

    while (run()) {

        queue_.wait_dequeue(bulk.front());
        consume(bulk);

        bulk.front().reset();
    }
}

template <>
void sink<multithread_queue_t>::loop() {
    std::vector<ptr_t> buffer;
    buffer.reserve(1024);

    on_start_thread();

    while (run()) {
        try {
            queue_.wait_dequeue_bulk_timed(std::back_inserter(buffer), buffer.capacity(), std::chrono::milliseconds(100));
            consume(buffer);
        }
        catch (std::exception& ex) {
            on_error(ex.what());
        }
        catch (...) {
            on_error("Unknown error");
        }
        buffer.clear();
    }

    on_end_thread();
}

template struct sink<multithread_queue_t>;
template struct sink<reader_writer_queue_t>;

} // namespace thread
} // namespace common