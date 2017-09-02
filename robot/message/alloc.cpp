#include "alloc.hpp"
#include <iostream>
#include <sstream>

/*
    \brief allocation mechanism for a multiple different types of objects with reduced time to live.
*/

namespace robot {
namespace message {

void buffer_type::increment_use_counter() {
    counter_.fetch_add(1, ORDER_WRITE);
}

bool buffer_type::decrement_use_counter() {

    unsigned counter_value = counter_.load(std::memory_order_relaxed);
    unsigned new_value;


    do {
        new_value = counter_value - 1;
    } while (!counter_.compare_exchange_weak(counter_value, new_value, ORDER_WRITE));

    return new_value == 0;
}

void buffer_pointer::reset() {
    if (instance_ != nullptr) {
        if (instance_->decrement_use_counter())
            delete instance_;
    }
}

buffer_pointer buffer_pointer::allocate() {
    buffer_pointer result;
    result.instance_ = new buffer_type();
    result->increment_use_counter();
    return result;
}

buffer_pointer::buffer_pointer()
: instance_(nullptr) {
}

buffer_pointer::buffer_pointer(const buffer_pointer& other) {
    instance_ = other.instance_;
    if (instance_ != nullptr)
        instance_->increment_use_counter();
}

buffer_pointer& buffer_pointer::operator=(buffer_pointer const& other) {
    reset();
    instance_ = other.instance_;
    instance_->increment_use_counter();

    return *this;
}

buffer_pointer& buffer_pointer::operator=(buffer_pointer&& other) {
    reset();
    instance_       = other.instance_;
    other.instance_ = nullptr;

    return *this;
}

buffer_pointer::buffer_pointer(buffer_pointer&& other) {
    instance_       = other.instance_;
    other.instance_ = nullptr;
}

buffer_pointer::~buffer_pointer() {
    reset();
}


buffer_holder_type::buffer_holder_type()
: pointer_(buffer_pointer::allocate()) {
}

thread_local buffer_holder_type holder;
buffer_holder_type& buffer_holder_type::get_tl() {
    return holder;
}

buffer_holder_type::~buffer_holder_type() {
}

void buffer_holder_type::allocate_buffer() {
    pointer_ = buffer_pointer::allocate();
}

} // namespace message
} // namespace robot