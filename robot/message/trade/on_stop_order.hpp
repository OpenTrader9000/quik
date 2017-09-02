#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_stop_order : public common::message::base {

    enum { code = common::message::codes::ON_STOP_ORDER };

    on_stop_order()
    : base(code) {
    }
    ~on_stop_order() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot