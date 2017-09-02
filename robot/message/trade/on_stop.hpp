#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_stop : public robot::message::base {

    enum { code = codes::ON_STOP };

    on_stop()
    : base(code) {
    }
    ~on_stop() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot