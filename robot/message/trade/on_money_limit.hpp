#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_money_limit : public common::message::base {

    enum { code = common::message::codes::ON_MONEY_LIMIT };

    on_money_limit()
    : base(code) {
    }
    ~on_money_limit() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot