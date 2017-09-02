#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace trade {

struct on_depo_limit : public robot::message::base {

    enum { code = codes::ON_DEPO_LIMIT };

    on_depo_limit()
    : base(code) {
    }
    ~on_depo_limit() {
    }
};

} // namespace trade
} // namespace message
} // namespace robot