
#pragma once

#include "alloc.hpp"
#include "codes.hpp"
#include <assert.h>
#include <atomic>

namespace common {
namespace message {
struct base {

    using buffer_pointer_t = ::common::message::buffer_pointer;

    int code_;
    std::atomic_size_t counter_;
    buffer_pointer_t pointer_;

    base(int code);
    virtual ~base();
};



} // namespace message
} // namespace robot