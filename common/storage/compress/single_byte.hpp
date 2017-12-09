#pragma once


namespace common {
namespace storage {
namespace compress {

struct char_decenc {
    unsigned char value_;

    void compress(char value) { value_ = static_cast<unsigned char>(value); }

    void compress(unsigned char value) { value_ = value; }

    unsigned char decode_impl(unsigned char const*& buffer, unsigned& size) {
        value_ = *buffer;
        ++buffer;
        --size;
        return value_;
    }

    template <typename T>
    T decode(unsigned char const*& buffer, unsigned& size) {
        return static_cast<T>(decode_impl(buffer, size));
    }

    void write(unsigned char*& buffer, unsigned& to_end) {
        *buffer = value_;
        ++buffer;
        --to_end;
    }

    template<typename CharType>
    void setup(CharType value) {
        value_ = static_cast<unsigned char>(value);
    }
};

} // namespace compress
} // namespace storage
} // namespace common