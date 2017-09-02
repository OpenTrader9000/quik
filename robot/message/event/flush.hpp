#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace event {
struct flush : public robot::message::base {

    enum { code = codes::FLUSH };

    flush()
    : base(code) {
    }
};
} // namespace event
} // namespace message
} // namespace robot