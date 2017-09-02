
#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_futures_limit_delete : public common::message::base {

    enum { code = common::message::codes::ON_FUTURES_LIMIT_DELETE };

    on_futures_limit_delete()
    : base(code) {
    }
    ~on_futures_limit_delete() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot