#pragma once

#include "base.hpp"

namespace common {
namespace message {

template <typename T>
struct ptr_concrete;

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

    bool is_code(codes c) { return (ptr_ != nullptr) && (ptr_->code_ == static_cast<int>(c)); }

    //template <typename T>
    //T* cast() {
    //    assert(T::code == ptr_->code_);
    //    return reinterpret_cast<T*>(ptr_);
    //}

    template<typename T>
    ptr_concrete<T> extract();

    template<typename T>
    T* cast();


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

    ptr_concrete(){}

    ~ptr_concrete() {
    }

    T* operator->() {
        return reinterpret_cast<T*>(ptr_);
    }

    T& operator*() {
        return *reinterpret_cast<T*>(ptr_);
    }
};


template<typename T>
inline ptr_concrete<T> ptr::extract()
{
    ptr_concrete<T> result;

    assert(T::code == ptr_->code_);

    // workaround for avoiding atomic operations
    result.ptr_ = ptr_;
    ptr_ = nullptr;
    return result;
}

template<typename T>
inline T * ptr::cast()
{
    assert(T::code == ptr_->code_);
    return reinterpret_cast<T*>(ptr_);
}

template <typename MessageType, typename... Args>
ptr_concrete<MessageType> make(Args&&... args) {
    static_assert(std::is_base_of<base, MessageType>::value,
                  "Constructed type must be inherited message::base.");

    auto& holder   = buffer_holder_type::get_tl();

    MessageType* p = holder.construct<MessageType>(std::forward<Args>(args)...);
    p->pointer_    = holder.pointer_;

    return ptr_concrete<MessageType>(p);
}

} // namespace message
} // namespace common