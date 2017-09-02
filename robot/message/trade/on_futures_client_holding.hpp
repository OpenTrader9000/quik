#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_futures_client_holding : public common::message::base {

    enum { code = common::message::codes::ON_FUTURES_CLIENT_HOLDING };

    on_futures_client_holding()
    : base(code) {
    }
    ~on_futures_client_holding() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot