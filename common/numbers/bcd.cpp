#include "bcd.hpp"
#include <assert.h>

namespace common {
namespace numbers {

int64_t pow10[] = {
    1,
    10,
    100,
    1000,
    10000,
    100000,
    1000000,
    10000000,
    100000000,
    1000000000,
    10000000000,
    100000000000,
    1000000000000,
    10000000000000,
    100000000000000,
    1000000000000000,
};

bcd::bcd()
: fractional_size_(0)
, value_(0) {
}

bcd::bcd(double d, int power)
: fractional_size_(power) {

    assert(sizeof(pow10) > power);
    assert(power >= 0);
    
    sign_ = (d < 0 ? 1 : 0);
    double new_value = (d * pow10[power] + (sign_ == 1 ? -0.5 : 0.5));
    value_ = static_cast<uint64_t>(new_value * (sign_ == 1 ? -1 : 1));
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
    return static_cast<double>(value / pow10[fractional_size_]) * (sign_ == 1 ? -1 : 1);
}

float bcd::to_float() const {
    float value = static_cast<float>(value_);
    return static_cast<float>(value / pow10[fractional_size_]) * (sign_ == 1 ? -1 :1);
}

bool bcd::operator==(bcd const& other) const {
    assert(fractional_size_ == other.fractional_size_);
    return value_ == other.value_ && other.sign_ == sign_;
}

bool bcd::operator!=(bcd const& other) const {
    return !(*this == other);
}

bool bcd::operator<(bcd const& other) const {
	assert(fractional_size_ == other.fractional_size_);
	if (sign_ != other.sign_) {
		if (sign_ == 1)
			return true;
		return false;
	}
	
	return (sign_ == 1 ? value_ > other.value_ : value_ < other.value_);
}

bool bcd::operator>(bcd const& other) const {
	assert(fractional_size_ == other.fractional_size_);
	if (sign_ != other.sign_) {
		if (sign_ == 1)
			return false;
		return true;
	}

	return (sign_ == 1 ? value_ < other.value_ : value_ > other.value_);
}



bcd& bcd::operator+(bcd const& other) {
    assert(fractional_size_ == other.fractional_size_);
    if (sign_ == other.sign_) {
        value_ += other.value_;
    }
    else if (value_ > other.value_) {
        value_ -= other.value_;; // sign will be the same
    }
    else if (value_ < other.value_) {
        value_ = other.value_ - value_;
        sign_ = other.sign_;
    }
    else if (value_ == other.value_) {
        value_ = 0;
        sign_ = 0;
        fractional_size_ = 0;
    }
    
    return *this;
}

bcd& bcd::operator-(bcd const& other) {
    assert(fractional_size_ == other.fractional_size_);

    //temprorary value for an operation
    bcd tmp(other);
    tmp.sign_ = (tmp.sign_ == 1 ? 0 : 1);

    *this = (*this + tmp);
    return *this;
}

bcd& bcd::operator=(bcd const& other) {
    fractional_size_ = other.fractional_size_;
    value_           = other.value_;
    sign_            = other.sign_;
    return *this;
}

void bcd::clear() {
    fractional_size_ = 0;
    value_           = 0;
    sign_            = 0;
}

inline bool is_number(char c) {
    return (c >= '0') && (c <= '9');
}

inline bool is_dot(char c) {
    return (c == '.') || (c == ',');
}

void bcd::parse(char const* str, int power) {

    if (!*str) {
        clear();
        return;
    }

    bool parsing_fractional = false;
    char const* pointer   = str;

    // parse sign
    sign_ = (*pointer == '-' ? 1 : 0);
    if (sign_ == 1)
        ++pointer;
    
    while (*pointer) {
        char c = *pointer;
        if (is_number(c)) {
            
            //prevent any possible next parse operations
            if (parsing_fractional && power == fractional_size_) {
                return;
            }

            value_ = value_ * 10 + (c - '0');

            if (parsing_fractional) {
                ++fractional_size_;
            }

        } else if (is_dot(c)) {
            // in the number might be present just a single . or ,
            assert(!parsing_fractional);
            parsing_fractional = true;
        } else
            // check for garbage
            assert(false);
        ++pointer;
    }
}


inline char extract_char(uint64_t& value) {
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

    while (fractional > 0) {
        char ch = extract_char(value);
        write_char(result.str, pointer, ch);
        --fractional;
    }

    // this means that present at least one single character in fractional part
    if (pointer > 0)
        write_char(result.str, pointer, '.');

    do {
        // extract at least one character
        char ch = extract_char(value);
        write_char(result.str, pointer, ch);
    } while (value > 0);

    if (sign_ == 1)
        write_char(result.str, pointer, '-');

    std::reverse(result.str, result.str + pointer);
    return result;
}

uint64_t bcd::as_uint64_t() const
{
    uint64_t result = 0;
    result += (sign_ << 63);
    result += (fractional_size_ << 59);
    result += value_;
    return result;
}

void bcd::from_uint64_t(uint64_t value)
{
    sign_ = (value >> 63);
    fractional_size_ = (value >> 59) & 0xF;
    value_ = value & 0x07FFFFFFFFFFFFFF;
}


bcd parse_bcd(char const* value) {
    bcd result{};
    result.parse(value);
    return result;
}


bcd parse_bcd(char const* value, int power) {
    bcd result{};
    result.parse(value, power);
    return result;
}

bcd min_bcd(bcd const& b){
    bcd result{};
    result.fractional_size_ = b.fractional_size_;
    return result;
}
bcd max_bcd(bcd const& b){
    bcd result{};
    result.value_ = std::numeric_limits<decltype(result.value_)>::max();
    result.fractional_size_ = b.fractional_size_;
    return result;
}

} // namespace numbers
} // namespace common