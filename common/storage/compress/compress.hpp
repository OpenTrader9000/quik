#pragma once

#include <common/container/array_view.hpp>
#include <common/debug/value.hpp>
#include <utils/meta/apply.hpp>
#include <assert.h>

// for consistency
#include "make.hpp"

namespace common {
namespace storage {
namespace compress {

template <typename OriginalType, typename Compressors, typename Callback>
inline void encode(OriginalType& value, Compressors& compressors, Callback&& f) {
    constexpr unsigned reserved_size = sizeof(OriginalType) * 2;
    unsigned char      tmp_buf[reserved_size];

    unsigned char* pointer = tmp_buf;
    unsigned       size    = reserved_size;

    utils::apply_tuple(
        [&](auto& field) mutable {
            field.encode(value, pointer, size);
        },
        compressors);

    f(common::container::array_view<unsigned char>(tmp_buf, reserved_size - size));
}

template <typename OriginalType, typename Compressors>
inline void decode(OriginalType& value, Compressors& compressors, unsigned char const*& buffer, unsigned& size) {

    // this code will be walk throught many different functions
    // this checks will help with wrong behaviour
    debug::value<unsigned char const*> saved_buffer(buffer);
    debug::value<unsigned>             saved_size(size);

    // TODO: Make checks for out of range errors
    utils::apply_tuple(
        [&](auto& field) mutable {
            field.decode(value, buffer, size);
        },
        compressors);

    assert(saved_buffer.diff(buffer) == saved_size.reverse_diff(size));
}

template<typename Compressors, typename OriginalType>
inline void set_compressor_state(Compressors& compressors, OriginalType const& value) {
    utils::apply_tuple([&](auto& field) { field.setup(value); }, compressors);
}

} // namespace compress
} // namespace storage
} // namespace common