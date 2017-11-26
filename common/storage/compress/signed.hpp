#pragma once

#include "unsigned.hpp"

#include <common/numbers/cast.hpp>

namespace common {
namespace storage {
namespace compress {

struct signed_encoder : public unsigned_encoder {

    signed_encoder& compress(int64_t value) {

		constexpr unsigned char sign = 64; // 01000000

        bool     is_negative  = (value < 0);
        uint64_t compressable = (is_negative ? -value : value);

        compute_bytes_count_and_alloc(compressable * 2); // sing bit
        compress_impl(compressable);

		// mark sign bit
        if (is_negative) {
            buffer_[0] = buffer_[0] | sign;
        }

        return *this;
    }
};


struct signed_decoder {
    signed_decoder() {}

    int64_t decode_impl(unsigned char*& buffer, unsigned& size) {
        enum {
            BIT  = 128, // 10000000
            MASK = 127, // 01111111,
            SIGN = 64   // 01000000
        };

        bool is_negative = ((buffer[0] & SIGN) != 0);
        if (is_negative)
            buffer[0] = ~(SIGN)&buffer[0];

        int64_t result = 0;

        auto          pointer = buffer - 1;
        unsigned char value   = 0;
        do {
            ++pointer;
            value = *pointer;
            result *= 128;          // shift
            result += value & MASK; // write tail

            ++buffer;
            --size;

        } while ((value & BIT) != 0);

        if (is_negative)
            result *= -1;

        return result;
    }

    template<typename T>
    T decode(unsigned char*& buffer, unsigned size) {
        int64_t dec = decode_impl(buffer, size);
        return numbers::integer_cast<T>(dec);
    }

};
} // namespace compress
} // namespace storage
} // namespace common