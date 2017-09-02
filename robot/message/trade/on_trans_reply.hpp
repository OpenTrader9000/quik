#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_trans_reply : public robot::message::base {

    enum { code = codes::ON_TRANS_REPLY };

    on_trans_reply()
    : base(code) {
    }
    ~on_trans_reply() {
    }

    int64_t trans_id_;
    int status_;
    int64_t time_;
    int64_t uid_;
    int64_t server_trans_id_;
};

} // namespace trade
} // namespace message
} // namespace robot