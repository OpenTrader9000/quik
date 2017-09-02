
#pragma once

#include "alloc.hpp"
#include "codes.hpp"
#include <assert.h>
#include <atomic>

namespace common {
namespace message {
struct base {
    int code_;
    std::atomic_size_t counter_;
    buffer_pointer pointer_;

    base(int code);
    virtual ~base();
};

struct ptr {
    ptr();
    ptr(base* pointer);
    ptr(ptr const& other);
    ptr(ptr&& other);
    ~ptr();

    explicit operator bool() const {
        return ptr_ != nullptr;
    }

    ptr& operator=(ptr const& other);

    base* operator->() {
        return ptr_;
    }

    template <typename T>
    T* cast() {
        assert(T::code == ptr_->code_);
        return reinterpret_cast<T*>(ptr_);
    }

    void reset();

    protected:
    void increment();
    size_t decrement();

    base* ptr_;
};

template <typename T>
struct ptr_concrete : public ptr {
    ptr_concrete(T* value)
    : ptr(value) {
    }
    ~ptr_concrete() {
    }

    T* operator->() {
        return reinterpret_cast<T*>(ptr_);
    }

    T& operator*() {
        return *reinterpret_cast<T*>(ptr_);
    }
};

template <typename MessageType, typename... Args>
ptr_concrete<MessageType> make(Args&&... args) {
    static_assert(std::is_base_of<base, MessageType>::value,
                  "Constructed type must be inherited message::base.");
    auto& holder   = buffer_holder_type::get_tl();
    MessageType* p = holder.construct<MessageType>(std::forward<Args>(args)...);
    p->pointer_    = holder.pointer_;
    return ptr_concrete<MessageType>(p);

    // return ptr_concrete<MessageType>(new MessageType(std::forward<Args>(args)...));
}


} // namespace message
} // namespace robot