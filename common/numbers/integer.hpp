#pragma once

#include <stdint.h>

namespace common {
namespace numbers {

int parse_int(char const* str);
unsigned parse_uint(char const* str);
int64_t parse_int64(char const* str);
uint64_t parse_uint64(char const* str);

} // namespace numbers
} // namespace common