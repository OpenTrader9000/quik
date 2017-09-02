#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace common {
namespace message {
namespace event {
struct flush : public common::message::base {

    enum { code = codes::FLUSH };

    flush()
    : base(code) {
    }
};
} // namespace event
} // namespace message
} // namespace common
