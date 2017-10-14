#pragma once


#include "signed.hpp"


namespace common {
namespace storage {
namespace compress {

// history decoder allways use signed integer diff which writes into buffer
// maximum possible difference is max(uint64_t)/2
struct unsigned_encoder_with_history : public integer_encoder {

    uint64_t last_value_ = 0;

    void compress(uint64_t new_value) {
        bool is_neg = (new_value < last_value_);

        // checks for too big value or negative value
        uint64_t diff = (is_neg ? last_value_ - new_value : new_value - last_value_);
        assert(diff < std::numeric_limits<uint64_t>::max() / 2);
        if (diff >= std::numeric_limits<uint64_t>::max() / 2)
            throw std::runtime_error("Cannot serialize unsigned integer");

        int64_t diffProp = static_cast<int64_t>(diff) * (is_neg ? -1 : 1);
        integer_encoder::compress(diffProp);
        last_value_ = new_value;
    }
};


struct unsigned_decoder_with_history : public integer_decoder {

    uint64_t last_value_ = 0;

    uint64_t decode(unsigned char*& buffer, unsigned& size) {

        // get a difference and check if sum last_value and difference is non negative
        int64_t diff = integer_decoder::decode(buffer, size);
        if (diff < 0 && std::abs(diff) > last_value_) {
            throw std::runtime_error("Difference has too low value");
        }
        last_value_ += diff;
        return last_value_;
    }

    unsigned decode32(unsigned char*& buffer, unsigned& size) {
        uint64_t value = decode(buffer, size);
        if (value > std::numeric_limits<unsigned>::max()) {
            throw std::runtime_error("Value too big for converting to unsigned int");
        }
        return static_cast<unsigned>(value);
    }
};
} // namespace compress
} // namespace storage
} // namespace common