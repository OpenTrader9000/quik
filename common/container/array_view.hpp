#pragma once

#include <vector>


namespace common {
namespace container {

template <typename T>
struct array_view_iterator {
    typedef ptrdiff_t difference_type;
    typedef std::forward_iterator_tag iterator_category;
    typedef T* pointer;
    typedef T& reference;
    typedef T value_type;

    array_view_iterator(T* ptr)
    : ptr_(ptr) {}

    bool operator!=(array_view_iterator const& other) const { return ptr_ != other.ptr_; }
    bool operator==(array_view_iterator const& other) const { return ptr_ == other.ptr_; }

    array_view_iterator& operator++() {
        ++ptr_;
        return *this;
    }

    array_view_iterator& operator++(int) {
        ++ptr_;
        return array_view_iterator(ptr_ - 1);
    }

    T& operator*() const { return *ptr_; }

 private:
    T* ptr_;
};

template <typename T>
struct array_view {

    array_view(T* ptr, unsigned size)
    : ptr_(ptr)
    , size_(size) {}

    array_view(std::vector<T>& vec)
    : ptr_(vec.data())
    , size_(vec.size()) {}

    array_view(array_view const& other) {
        ptr_  = other.ptr_;
        size_ = other.size_;
    }

    ~array_view() {}

    unsigned size() const { return size_; }

    array_view_iterator<T> begin() { return array_view_iterator<T>(ptr_); }
    array_view_iterator<T> end() { return array_view_iterator<T>(ptr_ + size_); }

    array_view_iterator<const T> begin() const { return array_view_iterator<const T>(ptr_); }
    array_view_iterator<const T> end() const { return array_view_iterator<const T>(ptr_ + size_); }

 private:
    T* ptr_;
    unsigned size_;
};
} // namespace container
} // namespace common