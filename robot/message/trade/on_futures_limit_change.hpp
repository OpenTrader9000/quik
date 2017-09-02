#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_futures_limit_change : public common::message::base {

    enum { code = common::message::codes::ON_FUTURES_LIMIT_CHANGE };

    on_futures_limit_change()
    : base(code) {
    }
    ~on_futures_limit_change() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot