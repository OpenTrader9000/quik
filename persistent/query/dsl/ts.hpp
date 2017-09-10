 #pragma once

// timestamp

#include <stdint.h>


namespace persistent {
namespace query {
namespace dsl {

struct ts {
    int64_t ts_ = 0;

    ts() {
    }

    ts(unsigned timestamp)
    : ts_(timestamp) {
    }

    template <typename T>
    void operator()(T& query) {
        query.ts_ = ts_;
    }
};

struct ts_range {
    int64_t low_ts_  = 0;
    int64_t high_ts_ = 0;

    ts_range() {
    }

    ts_range(int64_t low, int64_t high)
    : low_ts_(low)
    , high_ts_(high) {
    }

    template <typename T>
    void operator()(T& query) {
        query.ts_range_ = *this;
    }
};

} // namespace dsl
} // namespace query
} // namespace persistent