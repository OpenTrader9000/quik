#pragma once

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <common/container/array_view.hpp>

namespace common {
namespace storage {
namespace compress {
struct unsigned_encoder {

    unsigned char count_of_bytes_;
    unsigned char buffer_[10];

    unsigned char size() { return count_of_bytes_; }

    // TODO: Return array view
    void write(unsigned char*& buffer, unsigned& to_end) {
        std::copy(buffer_, buffer_ + count_of_bytes_, buffer);
        buffer += count_of_bytes_; // shift array
        to_end -= count_of_bytes_; // shift array
    }

    container::array_view<unsigned char> data() {
        return container::array_view<unsigned char>(buffer_, count_of_bytes_);
    }


    void compute_bytes_count_and_alloc(uint64_t value) {
        count_of_bytes_         = bytes_count(value);
        unsigned char tmp_bytes = count_of_bytes_;

        while (--tmp_bytes != 0) {
            buffer_[count_of_bytes_ - tmp_bytes - 1] = (1 << 7);
        }

        buffer_[count_of_bytes_ - 1] = 0;
    }

    unsigned char bytes_count(uint64_t value) {

        // different type of compression
        // 00000000
        // 10000000 00000000
        // 10000000 10000000 00000000
        // 10000000 10000000 10000000 00000000
        // 10000000 10000000 10000000 10000000 00000000
        enum : uint64_t {
            ONE_BYTE_LIMIT   = (1ull << 7),
            TWO_BYTE_LIMIT   = (1ull << 7 + 7),
            THREE_BYTE_LIMIT = (1ull << 7 + 7 + 7),
            FOUR_BYTE_LIMIT  = (1ull << 7 + 7 + 7 + 7),
            FIVE_BYTE_LIMIT  = (1ull << 7 + 7 + 7 + 7 + 7),
            SIX_BYTE_LIMIT   = (1ull << 7 + 7 + 7 + 7 + 7 + 7),
            SEVEN_BYTE_LIMIT = (1ull << 7 + 7 + 7 + 7 + 7 + 7 + 7),
            EIGHT_BYTE_LIMIT = (1ull << 7 + 7 + 7 + 7 + 7 + 7 + 7 + 7),
            NINE_BYTE_LIMIT  = (1ull << 7 + 7 + 7 + 7 + 7 + 7 + 7 + 7 + 7),
        };

        // less possible
        if (value < ONE_BYTE_LIMIT)
            return 1;
        if (value < TWO_BYTE_LIMIT)
            return 2;
        if (value < THREE_BYTE_LIMIT)
            return 3;
        if (value < FOUR_BYTE_LIMIT)
            return 4;
        if (value < FIVE_BYTE_LIMIT)
            return 5;
        if (value < SIX_BYTE_LIMIT)
            return 6;
        if (value < SEVEN_BYTE_LIMIT)
            return 7;
        if (value < EIGHT_BYTE_LIMIT)
            return 8;
        if (value < NINE_BYTE_LIMIT)
            return 9;
        return 10;
    }

    unsigned_encoder& compress(uint64_t value) {
        compute_bytes_count_and_alloc(value);
        return compress_impl(value);
    }

 protected:
    unsigned_encoder& compress_impl(uint64_t value) {

        constexpr unsigned char mask = 128 - 1;

        unsigned index     = count_of_bytes_;
        unsigned tmp_value = value;
        while (index-- != 0) {
            unsigned char v = tmp_value & mask;
            buffer_[index]  = buffer_[index] | v;

            tmp_value >>= 7;
        }

        return *this;
    }
};


struct unsigned_decoder {
    unsigned_decoder() {}

    uint64_t decode(unsigned char*& buffer, unsigned& size) {
        enum {
            BIT  = 128, // 10000000
            MASK = 127  // 01111111
        };

        uint64_t result = 0;

        auto          pointer = buffer - 1;
        unsigned char value   = 0;
        do {
            ++pointer;

            result *= 128; // shift
            value = *pointer;
            result += value & MASK; // write tail

            ++buffer;
            --size;

        } while ((value & BIT) != 0);

        return result;
    }

    unsigned decode32(unsigned char*& buffer, unsigned& size) {
        uint64_t dec = decode(buffer, size);
        if (dec > std::numeric_limits<unsigned>::max())
            throw std::runtime_error("Numer to big for convering to unsigned int");
        return static_cast<unsigned>(dec);
    }
};
} // namespace compress
} // namespace storage
} // namespace common