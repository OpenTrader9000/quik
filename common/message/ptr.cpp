#include "ptr.hpp"

namespace common {
namespace message {


ptr::ptr()
: ptr_(nullptr) {
}

ptr::ptr(base* pointer)
: ptr_(pointer) {
    assert(pointer != nullptr);
    increment();
}

ptr::ptr(ptr const& other) {
    assert(other.ptr_ != nullptr);
    ptr_ = other.ptr_;
    increment();
}

ptr::ptr(ptr&& other) {
    assert(other.ptr_ != nullptr);
    ptr_       = other.ptr_;
    other.ptr_ = nullptr;
}

ptr& ptr::operator=(ptr const& other) {
    assert(other.ptr_ != nullptr);

    reset();

    ptr_ = other.ptr_;
    increment();
    return *this;
}

ptr::~ptr() {
    reset();
}

void ptr::reset() {
    if (ptr_ == nullptr)
        return;

    if (decrement() == 0)
        ptr_->~base();

    ptr_ = nullptr;
}

void ptr::increment() {
    ptr_->counter_.fetch_add(1, std::memory_order_release);
}

size_t ptr::decrement() {

    size_t counter_value = ptr_->counter_.load(std::memory_order_relaxed);
    size_t new_value;

    do {
        new_value = counter_value - 1;
    } while (!ptr_->counter_.compare_exchange_weak(counter_value, new_value, std::memory_order_release));

    return new_value;
}
} // namespace message
} // namespace common