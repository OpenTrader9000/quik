#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_account_position : public common::message::base {

    enum { code = common::message::codes::ON_ACCOUNT_POSITION };

    on_account_position()
    : base(code) {
    }
    ~on_account_position() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot