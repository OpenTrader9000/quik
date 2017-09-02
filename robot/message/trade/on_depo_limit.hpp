#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_depo_limit : public common::message::base {

    enum { code = common::message::codes::ON_DEPO_LIMIT };

    on_depo_limit()
    : base(code) {
    }
    ~on_depo_limit() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot