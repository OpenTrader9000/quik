#pragma once

#include <common/container/array_view.hpp>
#include <utils/meta/apply.hpp>

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
inline void decode(OriginalType& value, Compressors& compressors, unsigned char*& buffer, unsigned& size) {
    // TODO: Make checks for out of range errors
    utils::apply_tuple(
        [&](auto& field) mutable {
            field.decode(value, buffer, size);
        },
        compressors);
}

} // namespace compress
} // namespace storage
} // namespace common