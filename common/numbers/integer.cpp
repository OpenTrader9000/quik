#include "integer.hpp"
#include <assert.h>

namespace common {

namespace numbers {

inline bool is_number(char c) {
    return (c >= '0') && (c <= '9');
}

template <typename Integer>
inline Integer parse(char const* str) {
    Integer result{};

    char const* pointer = str;
    while (*pointer) {
        char c = *pointer;
        assert(is_number(c));
        result = result * 10 + (c - '0');
        ++pointer;
    }
    return result;
}

template <typename Integer>
inline Integer parse_signed_int(char const* str) {
    int multiplier = 1;
    if (*str == '-') {
        multiplier = -1;
        ++str;
    }
    return parse<Integer>(str) * multiplier;
}

int parse_int(char const* str) {
    return parse_signed_int<int>(str);
}

unsigned parse_uint(char const* str) {
    return parse<unsigned>(str);
}

int64_t parse_int64(char const* str) {
    return parse_signed_int<int64_t>(str);
}

uint64_t parse_uint64(char const* str) {
    return parse<uint64_t>(str);
}
} // namespace numbers
} // namespace common