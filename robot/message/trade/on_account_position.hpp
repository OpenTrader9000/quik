#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_account_position : public robot::message::base {

    enum { code = codes::ON_ACCOUNT_POSITION };

    on_account_position()
    : base(code) {
    }
    ~on_account_position() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot