#pragma once

#include <blockingconcurrentqueue.h>
#include <common/message/base.hpp>
#include <readerwriterqueue.h>

namespace robot {
namespace thread {

using reader_writer_queue_t = moodycamel::BlockingReaderWriterQueue<common::message::ptr>;
using multithread_queue_t   = moodycamel::BlockingConcurrentQueue<common::message::ptr>;

} // namespace thread
} // namespace robot