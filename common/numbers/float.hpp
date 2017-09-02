
#pragma once
#include "bcd.hpp"

namespace common {
namespace numbers {
inline float parse_float(char const* str) {
    return parse_bcd(str).to_float();
}

inline double parse_double(char const* str) {
    return parse_bcd(str).to_double();
}
} // namespace numbers
} // namespace common