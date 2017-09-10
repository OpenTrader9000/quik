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
    if (thread_.joinable())
        thread_.join();
}

template <typename QueueType>
void sink<QueueType>::start_sink() {
    thread_ = std::thread(&sink<QueueType>::loop, this);
}

template <typename QueueType>
void sink<QueueType>::push(::common::message::ptr&& message) {
    queue_.enqueue(std::move(message));
}

template <>
void sink<reader_writer_queue_t>::loop() {
    while (run()) {
        ptr_t mes;
        queue_.wait_dequeue(mes);
        consume(std::move(mes));
    }
}

template <>
void sink<multithread_queue_t>::loop() {
    std::vector<ptr_t> buffer;
    buffer.reserve(1024);
    while (run()) {
        queue_.wait_dequeue_bulk(std::back_inserter(buffer), buffer.capacity());
        for (ptr_t& mes : buffer)
            consume(std::move(mes));
        buffer.clear();
    }
}

template struct sink<multithread_queue_t>;
template struct sink<reader_writer_queue_t>;

} // namespace thread
} // namespace common