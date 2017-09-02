#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_all_trade : public common::message::base {

    enum { code = common::message::codes::ON_ALL_TRADE };

    on_all_trade()
    : base(code) {
    }
    ~on_all_trade() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot