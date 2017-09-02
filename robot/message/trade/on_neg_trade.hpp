#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_neg_trade : public robot::message::base {

    enum { code = codes::ON_NEG_TRADE };

    on_neg_trade()
    : base(code) {
    }
    ~on_neg_trade() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot