#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_all_trade : public robot::message::base {

    enum { code = codes::ON_ALL_TRADE };

    on_all_trade()
    : base(code) {
    }
    ~on_all_trade() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot