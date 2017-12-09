#pragma once

#include <algorithm>
#include <exception>
#include <stdexcept>
#include <assert.h>
#include <common/container/array_view.hpp>
#include <common/numbers/cast.hpp>

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


    void compute_bytes_count_and_alloc(uint64_t value);

    unsigned char bytes_count(uint64_t value);

    unsigned_encoder& compress(uint64_t value) {
        compute_bytes_count_and_alloc(value);
        return compress_impl(value);
    }

 protected:
     unsigned_encoder& compress_impl(uint64_t value);
};


struct unsigned_decoder {
    unsigned_decoder() {}

    uint64_t decode_uint_impl(unsigned char const*& buffer, unsigned& size);

    template<typename T>
    T decode(unsigned char const*& buffer, unsigned& size) {
        uint64_t dec = decode_uint_impl(buffer, size);
        return numbers::integer_cast<T>(dec);
    }

    void setup(uint64_t v){}
};
} // namespace compress
} // namespace storage
} // namespace common