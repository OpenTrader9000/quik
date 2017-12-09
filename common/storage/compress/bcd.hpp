#pragma once

#include "unsigned.hpp"
#include "unsigned_history.hpp"

#include <common/numbers/bcd.hpp>

namespace common {
namespace storage {
namespace compress {

struct bcd_encoder : public unsigned_encoder {
    void compress(common::numbers::bcd const& value) {
        unsigned_encoder::compress(value.as_uint64_t());
    }
};

struct bcd_encoder_with_history : public unsigned_encoder_with_history {
    void compress(common::numbers::bcd const& value) {
        unsigned_encoder_with_history::compress(value.as_uint64_t());
    }
};

struct bcd_decoder : public unsigned_decoder {
    template <typename T>
    T decode(unsigned char const*& buffer, unsigned& size);

    template <>
    common::numbers::bcd decode<common::numbers::bcd>(unsigned char const*& buffer, unsigned& size) {
        common::numbers::bcd result;
        uint64_t             value = unsigned_decoder::decode_uint_impl(buffer, size);
        result.from_uint64_t(value);
        return result;
    }

    void setup(common::numbers::bcd const&) {
    }
};

struct bcd_decoder_with_history : public unsigned_decoder_with_history {
    template <typename T>
    T decode(unsigned char const*& buffer, unsigned& size);

    template <>
    common::numbers::bcd decode<common::numbers::bcd>(unsigned char const*& buffer, unsigned& size) {
        common::numbers::bcd result;
        uint64_t             value = unsigned_decoder_with_history::decode_uint_impl(buffer, size);
        result.from_uint64_t(value);
        return result;
    }

    void setup(common::numbers::bcd const& value) {
        unsigned_decoder_with_history::setup(value.as_uint64_t());
    }
};

} // namespace compress
} // namespace storage
} // namespace common