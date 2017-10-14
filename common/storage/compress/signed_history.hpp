#pragma once

#include "signed.hpp"


namespace common {
namespace storage {
namespace compress {

struct integer_encoder_with_history : public integer_encoder {
    int64_t last_value_ = 0;

    void compress(int64_t new_value) {
        int64_t diff = (new_value - last_value_);
        integer_encoder::compress(diff);
        last_value_ = new_value;
    }
};


struct signed_decoder_with_history : public integer_decoder {
    int64_t last_value_ = 0;
    int64_t decode(unsigned char*& buffer, unsigned& size) {
        last_value_ += integer_decoder::decode(buffer, size);
        return last_value_;
    }

    int decode32(unsigned char*& buffer, unsigned& size) {
        int64_t value = decode(buffer, size);
        if (value > std::numeric_limits<int>::max() || value < std::numeric_limits<int>::max()) {
            throw std::runtime_error("Value too big or too low for converting to int");
        }
        return static_cast<int>(value);
    }
};
} // namespace compress
} // namespace storage
} // namespace common