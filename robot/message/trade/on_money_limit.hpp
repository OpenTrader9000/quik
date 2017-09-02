#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_money_limit : public robot::message::base {

    enum { code = codes::ON_MONEY_LIMIT };

    on_money_limit()
    : base(code) {
    }
    ~on_money_limit() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot