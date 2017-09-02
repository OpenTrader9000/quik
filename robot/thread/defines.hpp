#pragma once

#include <blockingconcurrentqueue.h>
#include <message/base.hpp>
#include <readerwriterqueue.h>

namespace robot {
namespace thread {

using reader_writer_queue_t = moodycamel::BlockingReaderWriterQueue<message::ptr>;
using multithread_queue_t   = moodycamel::BlockingConcurrentQueue<message::ptr>;

} // namespace thread
} // namespace robot