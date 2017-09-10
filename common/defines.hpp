#pragma once

#include <common/message/message.hpp>
#include <third_party/concurrentqueue/blockingconcurrentqueue.h>
#include <third_party/readerwriterqueue/readerwriterqueue.h>

namespace common {

using reader_writer_queue_t = moodycamel::BlockingReaderWriterQueue<common::message::ptr>;
using multithread_queue_t   = moodycamel::BlockingConcurrentQueue<common::message::ptr>;

} // namespace common