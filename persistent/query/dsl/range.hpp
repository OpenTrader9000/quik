#pragma once
#include <string>

#define DEFINE_DSL_RANGE(RANGE_NAME, RANGE_TYPE)                  \
struct RANGE_NAME##Range {                                        \
        RANGE_TYPE low_{};                                        \
        RANGE_TYPE high_{};                                       \
                                                                  \
                                                                  \
        RANGE_NAME() {                                            \
        }                                                         \
                                                                  \
        RANGE_NAME(RANGE_TYPE const& low, RANGE_TYPE const& high) \
        : low_(low)                                               \
        , high_(high) {                                           \
        }                                                         \
                                                                  \
        template <typename T>                                     \
        void operator()(T& query) {                               \
            query.RANGE_NAME = *this;                             \
        }                                                         \
 };
