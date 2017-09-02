#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace common {
namespace message {
namespace event {
struct stop : public base {

    enum { code = codes::STOP };

    stop()
    : base(code) {
    }
};
} // namespace event
} // namespace message
} // namespace robot