#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace common {
namespace message {
namespace event {
struct compress : public base {

    enum { code = codes::COMPRESS };

    compress()
        : base(code) {}
};
} // namespace event
} // namespace message
} // namespace common