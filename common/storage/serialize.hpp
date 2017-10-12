#pragma once

#include <iterator>

namespace common {
namespace storage {
template <typename T>
inline void serialize_impl(std::vector<char>& vector, T const& value) {
    char const* c = reinterpret_cast<char const*>(&value);
    std::copy(c, c + sizeof(value), std::back_inserter(vector));
}

template <typename T>
inline void deserialize_impl(char*& position, T& value) {
    value = *reinterpret_cast<T*>(position);
    position += sizeof(value);
}

template <typename... Args>
inline void serialize(std::vector<char>& vector, Args&... args) {
    using swallow = int[];
    (void)swallow{ (serialize_impl(vector, args), 0)... };
}

template <typename... Args>
inline int deserialize(char*& pointer, Args&... args) {
    char* ptr_start = pointer;
    using swallow   = int[];
    (void)swallow{ (deserialize_impl(pointer, args), 0)... };
    return pointer - ptr_start;
}
} // namespace storage
} // namespace common