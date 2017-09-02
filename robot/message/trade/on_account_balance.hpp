#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_account_balance : public robot::message::base {

    enum { code = codes::ON_ACCOUNT_BALANCE };

    on_account_balance()
    : base(code) {
    }
    ~on_account_balance() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot