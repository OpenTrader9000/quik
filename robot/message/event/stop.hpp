#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
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