#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_futures_limit_change : public robot::message::base {

    enum { code = codes::ON_FUTURES_LIMIT_CHANGE };

    on_futures_limit_change()
    : base(code) {
    }
    ~on_futures_limit_change() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot