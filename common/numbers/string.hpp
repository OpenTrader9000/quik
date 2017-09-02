#pragma once
#include <string>

namespace common {
namespace numbers {

struct string {

    char str[24] = {};

    operator char const*() const {
        return str;
    }

    operator std::string() const {
        return str;
    }
};
} // namespace numbers
} // namespace common