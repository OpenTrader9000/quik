#pragma once

#include "string.hpp"
#include <stdint.h>
#include <type_traits>

namespace common {

namespace numbers {


struct bcd {
    uint64_t sign_ : 1;
    uint64_t fractional_size_ : 4;
    uint64_t value_ : 59;

    bcd();
    bcd(double d, int power);
    bcd(bcd const&) = default;
    bcd(bcd&&) = default;
    bcd(char const*);
    ~bcd(){}

    operator float() const;
    operator double() const;

    double to_double() const;
    float to_float() const;
    string to_string() const;

    bool operator==(bcd const& other) const;
    bool operator!=(bcd const& other) const;
    bcd& operator+(bcd const& other);
    bcd& operator-(bcd const& other);
    bcd& operator=(bcd const& other);

    template <typename IntegerType>
    bcd& operator*(IntegerType multiplier);

    template <typename IntegerType>
    bcd& operator/(IntegerType divider);

    void clear();
    void parse(char const* str, int power = -1);

};

template <typename IntegerType>
inline bcd& bcd::operator*(IntegerType multiplier) {
    static_assert(std::is_integral<IntegerType>::value, "Multiplicator must be integer");
    value_ *= multiplier;
    return *this;
}

template <typename IntegerType>
inline bcd& bcd::operator/(IntegerType divider) {
    static_assert(std::is_integral<IntegerType>::value, "Divider must be integer");
    assert(divider != 0);
    value_ /= divider;
    return *this;
}

bcd parse_bcd(char const* value);
bcd parse_bcd(char const* value, int power);

} // namespace numbers
} // namespace common
using unibcd_t = common::numbers::bcd;
inline unibcd_t parse_bcd(char const* value) {
    return common::numbers::parse_bcd(value);
}
inline unibcd_t parse_bcd(char const* value, int power) {
    return common::numbers::parse_bcd(value, power);
}
