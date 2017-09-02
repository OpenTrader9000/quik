#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_neg_deal : public robot::message::base {

    enum { code = codes::ON_NEG_DEAL };

    on_neg_deal()
    : base(code) {
    }
    ~on_neg_deal() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot