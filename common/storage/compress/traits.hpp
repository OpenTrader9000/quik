#pragma once

#include "integer.hpp"
#include "single_byte.hpp"
#include "bcd.hpp"

namespace common {
namespace storage {
namespace compress {

template <typename CompressableType>
struct compress_traits;

// signed
template <>
struct compress_traits<char> {
    using history_encoder_t = char_decenc;
    using history_decoder_t = char_decenc;
    using encoder_t         = char_decenc;
    using decoder_t         = char_decenc;
};

template <>
struct compress_traits<short> {
    using history_encoder_t = signed_encoder_with_history;
    using history_decoder_t = signed_decoder_with_history;
    using encoder_t         = signed_encoder;
    using decoder_t         = signed_decoder;
};

template <>
struct compress_traits<int> {
    using history_encoder_t = signed_encoder_with_history;
    using history_decoder_t = signed_decoder_with_history;
    using encoder_t         = signed_encoder;
    using decoder_t         = signed_decoder;
};

template <>
struct compress_traits<int64_t> {
    using history_encoder_t = signed_encoder_with_history;
    using history_decoder_t = signed_decoder_with_history;
    using encoder_t         = signed_encoder;
    using decoder_t         = signed_decoder;
};


// unsigned
template <>
struct compress_traits<unsigned char> {
    using history_encoder_t = char_decenc;
    using history_decoder_t = char_decenc;
    using encoder_t         = char_decenc;
    using decoder_t         = char_decenc;
};

template <>
struct compress_traits<unsigned short> {
    using history_encoder_t = unsigned_encoder_with_history;
    using history_decoder_t = unsigned_decoder_with_history;
    using encoder_t         = unsigned_encoder;
    using decoder_t         = unsigned_decoder;
};

template <>
struct compress_traits<unsigned int> {
    using history_encoder_t = unsigned_encoder_with_history;
    using history_decoder_t = unsigned_decoder_with_history;
    using encoder_t         = unsigned_encoder;
    using decoder_t         = unsigned_decoder;
};

template <>
struct compress_traits<uint64_t> {
    using history_encoder_t = unsigned_encoder_with_history;
    using history_decoder_t = unsigned_decoder_with_history;
    using encoder_t         = unsigned_encoder;
    using decoder_t         = unsigned_decoder;
};

template <>
struct compress_traits<common::numbers::bcd> {
    using history_encoder_t = bcd_encoder_with_history;
    using history_decoder_t = bcd_decoder_with_history;
    using encoder_t         = bcd_encoder;
    using decoder_t         = bcd_decoder;
};

} // namespace compress
} // namespace storage
} // namespace common