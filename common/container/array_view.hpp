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

    array_view_iterator operator++(int) {
        ++ptr_;
        return array_view_iterator(ptr_ - 1);
    }

    T& operator*() const { return *ptr_; }

 private:
    T* ptr_;
};

// TODO: test it
template <typename T>
struct array_view_reverse_iterator {
	typedef ptrdiff_t difference_type;
	typedef std::forward_iterator_tag iterator_category;
	typedef T* pointer;
	typedef T& reference;
	typedef T value_type;

	array_view_reverse_iterator(T* ptr)
		: ptr_(ptr) {}

	bool operator!=(array_view_reverse_iterator const& other) const { return ptr_ != other.ptr_; }
	bool operator==(array_view_reverse_iterator const& other) const { return ptr_ == other.ptr_; }

	array_view_iterator& operator++() {
		--ptr_;
		return *this;
	}

	array_view_iterator operator++(int) {
		--ptr_;
		return array_view_reverse_iterator(ptr_ + 1);
	}

	T& operator*() const { return *ptr_; }

private:
	T* ptr_;
};


template <typename T>
struct array_view {

    using value_type = T;

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

    array_view()
    : ptr_(nullptr)
    , size_(0) {}

    ~array_view() {}

    bool empty() const {
        return size_ == 0 || ptr_ == nullptr;
    }

    size_t size() const { return size_; }

    T& operator[](int index) { return *(ptr_ + index); }
    T const& operator[](int index) const { return *(ptr_ + index); }

    T& front() {
        assert(ptr_ != nullptr);
        return *ptr_;
    }

    T const& front() const{
        assert(ptr_ != nullptr);
        return *ptr_;
    }

    T& back() {
        assert(ptr_ != nullptr);
        return *(ptr_ + size_ - 1);
    }
    
    T const& back() const {
        assert(ptr_ != nullptr);
        return *(ptr_ + size_ - 1);
    }

    array_view_iterator<T> begin() { return array_view_iterator<T>(ptr_); }
    array_view_iterator<T> end() { return array_view_iterator<T>(ptr_ + size_); }

    array_view_iterator<const T> begin() const { return array_view_iterator<const T>(ptr_); }
    array_view_iterator<const T> end() const { return array_view_iterator<const T>(ptr_ + size_); }

    array_view_reverse_iterator<T> rbegin() { return array_view_iterator<T>(ptr_ + size_ - 1); }
    array_view_reverse_iterator<T> rend() { return array_view_iterator<T>(ptr_ - 1); }

    array_view_reverse_iterator<const T> rbegin() const {
        return array_view_iterator<const T>(ptr_ + size_ - 1);
    }
    array_view_reverse_iterator<const T> rend() const {
        return array_view_iterator<const T>(ptr_ - 1);
    }


 private:
    T* ptr_;
    size_t size_;
};
} // namespace container
} // namespace common