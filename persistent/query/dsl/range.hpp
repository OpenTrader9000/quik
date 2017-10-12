#pragma once
#include <string>


// this type of dsl might be stored in the class itself
#define DEFINE_DSL_RANGE(RANGE_NAME)                               \
template<typename type>                                            \
struct RANGE_NAME##_range_impl {                                   \
        type low_{};                                               \
        type high_{};                                              \
                                                                   \
                                                                   \
        RANGE_NAME##_range_impl(type const& low, type const& high) \
        : low_(low)                                                \
        , high_(high) {}                                           \
                                                                   \
        template <typename T>                                      \
        void operator()(T& query) {                                \
            query.RANGE_NAME##_low  = low_;                        \
            query.RANGE_NAME##_high = high_;                       \
        }                                                          \
    };                                                             \
template<typename type>                                            \
inline RANGE_NAME##_range_impl<type>                               \
    RANGE_NAME##_r(type const& low, type const& high) {            \
        return RANGE_NAME##_range_impl<type>(low, high);           \
    }
