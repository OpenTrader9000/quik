#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_neg_deal : public common::message::base {

    enum { code = common::message::codes::ON_NEG_DEAL };

    on_neg_deal()
    : base(code) {
    }
    ~on_neg_deal() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot