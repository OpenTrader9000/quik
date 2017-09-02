#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_account_balance : public common::message::base {

    enum { code = common::message::codes::ON_ACCOUNT_BALANCE };

    on_account_balance()
    : base(code) {
    }
    ~on_account_balance() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot