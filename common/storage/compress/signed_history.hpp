#pragma once

#include "signed.hpp"
#include <common/numbers/cast.hpp>


namespace common {
namespace storage {
namespace compress {

struct signed_encoder_with_history : public signed_encoder {
    int64_t last_value_ = 0;

    void compress(int64_t new_value) {
        int64_t diff = (new_value - last_value_);
        signed_encoder::compress(diff);
        last_value_ = new_value;
    }
};


struct signed_decoder_with_history : public signed_decoder {
    int64_t last_value_ = 0;
    int64_t decode_impl(unsigned char const*& buffer, unsigned& size) {
        last_value_ += signed_decoder::decode_impl(buffer, size);
        return last_value_;
    }

    template<typename T>
    T decode(unsigned char const*& buffer, unsigned& size) {
        int64_t value = decode_impl(buffer, size);
        return common::numbers::integer_cast<T>(value);
    }

    void setup(int64_t value) {
        last_value_ = value;
    }
};
} // namespace compress
} // namespace storage
} // namespace common