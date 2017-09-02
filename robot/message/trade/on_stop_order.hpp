#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_stop_order : public robot::message::base {

    enum { code = codes::ON_STOP_ORDER };

    on_stop_order()
    : base(code) {
    }
    ~on_stop_order() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot