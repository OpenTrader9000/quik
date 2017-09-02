#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_money_limit_delete : public robot::message::base {

    enum { code = codes::ON_MONEY_LIMIT_DELETE };

    on_money_limit_delete()
    : base(code) {
    }
    ~on_money_limit_delete() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot