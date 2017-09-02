#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_trans_reply : public common::message::base {

    enum { code = common::message::codes::ON_TRANS_REPLY };

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