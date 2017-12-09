#pragma once

namespace common {
namespace debug {
template <typename T>
struct value {

#ifdef _DEBUG
    T value_;
#endif
    value(T value) {
#ifdef _DEBUG
        value_ = value;
#elif
        (void)value;
#endif
    }

    long long diff(T other) {
#ifdef _DEBUG
        return other - value_;
#elif
        return 0;
#endif
    }

    long long reverse_diff(T other) {
#ifdef _DEBUG
        return value_ - other;
#elif
        return 0;
#endif
    }

    ~value() {}
};
} // namespace debug
} // namespace common