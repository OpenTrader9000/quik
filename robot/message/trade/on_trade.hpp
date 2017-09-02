#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_trade : public common::message::base {

    enum { code = common::message::codes::ON_TRADE };

    on_trade()
    : base(code) {
    }
    ~on_trade() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot