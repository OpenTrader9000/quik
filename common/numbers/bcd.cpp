#include "bcd.hpp"
#include <assert.h>

namespace common {
namespace numbers {

int64_t pow10[] = {
    1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000, 10000000000, 100000000000, 1000000000000, 10000000000000, 100000000000000, 1000000000000000
};

bcd::bcd()
: fractional_size_(0)
, value_(0) {
}

bcd::bcd(char const* bcd_str)
: fractional_size_(0)
, value_(0) {
    parse(bcd_str);
}

bcd::operator float() const {
    return to_float();
}
bcd::operator double() const {
    return to_double();
}

double bcd::to_double() const {
    double value = static_cast<double>(value_);
    return value / pow10[fractional_size_];
}

float bcd::to_float() const {
    float value = static_cast<float>(value_);
    return value / pow10[fractional_size_];
}

bool bcd::operator==(bcd const& other) const {
    assert(fractional_size_ == other.fractional_size_);
    return value_ == other.value_;
}

bool bcd::operator!=(bcd const& other) const {
    return !(*this == other);
}

bcd& bcd::operator+(bcd const& other) {
    assert(fractional_size_ == other.fractional_size_);
    value_ += other.value_;
    return *this;
}

bcd& bcd::operator-(bcd const& other) {
    assert(fractional_size_ == other.fractional_size_);
    value_ -= other.value_;
    return *this;
}

bcd& bcd::operator=(bcd const& other) {
    fractional_size_ = other.fractional_size_;
    value_           = other.value_;
    return *this;
}

void bcd::clear() {
    fractional_size_ = 0;
    value_           = 0;
}

inline bool is_number(char c) {
    return (c >= '0') && (c <= '9');
}

inline bool is_dot(char c) {
    return (c == '.') || (c == ',');
}

void bcd::parse(char const* str) {

    bool count_fractional = false;
    char const* pointer   = str;
    while (*pointer) {
        char c = *pointer;
        if (is_number(c)) {
            value_ = value_ * 10 + (c - '0');
            if (count_fractional)
                ++fractional_size_;
        } else if (is_dot(c)) {
            // in the number might be present just a single . or ,
            assert(!count_fractional);
            count_fractional = true;
        } else
            // check for garbage
            assert(false);
        ++pointer;
    }
}

inline char extract_char(int64_t& value) {
    auto new_value = value / 10;
    auto ch        = (value - new_value * 10) + '0';
    value          = new_value;
    return static_cast<char>(ch);
}

inline void write_char(char* ar, unsigned& pointer, char new_value) {
    *(ar + pointer) = new_value;
    ++pointer;
}

string bcd::to_string() const {
    string result;

    unsigned pointer = 0;
    auto fractional  = fractional_size_;
    auto value       = value_;
    int sign         = (value < 0 ? -1 : 1);

    // make proper sign
    value *= sign;

    while (fractional > 0) {
        char ch = extract_char(value);
        write_char(result.str, pointer, ch);
        --fractional;
    }

    if (pointer > 0)
        write_char(result.str, pointer, '.');

    do {
        char ch = extract_char(value);
        write_char(result.str, pointer, ch);
    } while (value > 0);

    if (sign < 0)
        write_char(result.str, pointer, '-');

    std::reverse(result.str, result.str + pointer);
    return result;
}


bcd parse_bcd(char const* value) {
    bcd result{};
    result.parse(value);
    return result;
}
} // namespace numbers
} // namespace common