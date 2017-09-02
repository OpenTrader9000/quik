#pragma once
#include <assert.h>
#include <atomic>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>


#define ORDER_READ std::memory_order_acquire
#define ORDER_WRITE std::memory_order_release


namespace common {
namespace message {
struct buffer_pointer;
struct buffer_type {

    enum { size = 1024 * 32 };

    char* data_[size];
    unsigned pointer_ = 0;


    buffer_type() {
    }

    // make this operations without alignment until this will be required
    template <typename T>
    bool has_place_for_type() {
        return (size - pointer_) >= sizeof(T);
    }

    template <typename T, typename... Args>
    T* construct(Args&&... args) {
        assert(has_place_for_type<T>());
        T* result = new (data_ + pointer_) T(std::forward<Args>(args)...);
        pointer_ += sizeof(T);
        return result;
    }

    private:
    friend struct buffer_pointer;

    std::atomic_uint counter_;
    void increment_use_counter();
    bool decrement_use_counter();
};

struct buffer_pointer {

    buffer_type* instance_;

    static buffer_pointer allocate();

    buffer_pointer();
    buffer_pointer(const buffer_pointer& other);
    buffer_pointer(buffer_pointer&&);
    ~buffer_pointer();

    buffer_pointer& operator=(buffer_pointer const& other);
    buffer_pointer& operator=(buffer_pointer&& other);

    void reset();

    buffer_type* operator->() const {
        return instance_;
    }
};

// using buffer_pointer = std::shared_ptr<buffer_type>;
static size_t counter   = 0;
static size_t m_counter = 0;
struct buffer_holder_type {

    static buffer_holder_type& get_tl();

    buffer_pointer pointer_;

    buffer_holder_type();
    ~buffer_holder_type();

    void allocate_buffer();

    template <typename T, typename... Args>
    T* construct(Args&&... args) {
        static_assert(sizeof(T) <= buffer_type::size,
                      "Size of type must be less or equal default buffer size.");

        if (!pointer_->has_place_for_type<T>()) {
            allocate_buffer();
        }

        return pointer_->construct<T>(std::forward<Args>(args)...);
    }
};
} // namespace message
} // namespace robot