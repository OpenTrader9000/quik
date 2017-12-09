
#include "signed.hpp"

namespace common {
namespace storage {
namespace compress {
int64_t signed_decoder::decode_impl(unsigned char const*& buffer, unsigned& size) {
    enum {
        BIT  = 128, // 10000000
        MASK = 127, // 01111111,
        SIGN = 64   // 01000000
    };

    bool          is_negative            = ((buffer[0] & SIGN) != 0);
    unsigned char initial_unsigned_value = ~(SIGN)&buffer[0];

    ++buffer;
    --size;

    int64_t result = initial_unsigned_value  & MASK;
    if ((initial_unsigned_value & BIT) == 0) {
        return result * (is_negative ? -1 : 1);
    }

    unsigned char value   = 0;

    do  {
        value = *buffer;
        result *= 128;          // shift
        result += value & MASK; // write tail

        ++buffer;
        --size;

    } while ((value & BIT) != 0);

    return result * (is_negative ? -1 : 1);
}

signed_encoder& signed_encoder::compress(int64_t value) {

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
} // namespace compress
} // namespace storage
} // namespace common