#pragma once

#include "unsigned.hpp"

#include <common/numbers/cast.hpp>

namespace common {
namespace storage {
namespace compress {

struct signed_encoder : public unsigned_encoder {

    signed_encoder& compress(int64_t value);
};


struct signed_decoder {
    signed_decoder() {}

    int64_t decode_impl(unsigned char const*& buffer, unsigned& size);

    template<typename T>
    T decode(unsigned char const*& buffer, unsigned& size) {
        int64_t dec = decode_impl(buffer, size);
        return numbers::integer_cast<T>(dec);
    }

    void setup(int64_t v){}

};
} // namespace compress
} // namespace storage
} // namespace common