#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_stop : public common::message::base {

    enum { code = common::message::codes::ON_STOP };

    on_stop()
    : base(code) {
    }
    ~on_stop() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot