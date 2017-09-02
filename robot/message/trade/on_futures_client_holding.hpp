#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_futures_client_holding : public robot::message::base {

    enum { code = codes::ON_FUTURES_CLIENT_HOLDING };

    on_futures_client_holding()
    : base(code) {
    }
    ~on_futures_client_holding() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot