#pragma once

#include <stdint.h>

class SkCanvas;

namespace view {
namespace primitive {

struct x_axis {

    uint64_t width_;
    uint64_t start_ts_;
    uint64_t end_ts_;

    x_axis(uint64_t width, uint64_t start_ts, uint64_t end_ts);
    ~x_axis();
};

} // namespace primitive
} // namespace view