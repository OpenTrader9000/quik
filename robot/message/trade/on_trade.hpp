#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_trade : public robot::message::base {

    enum { code = codes::ON_TRADE };

    on_trade()
    : base(code) {
    }
    ~on_trade() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot