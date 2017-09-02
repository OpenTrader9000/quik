#pragma once

#include <message/base.hpp>
#include <message/codes.hpp>

namespace robot {
namespace message {
namespace lua {

struct unhandled : public base {
    enum { code = codes::UNHANDLED };

    unhandled()
    : base(code) {
    }

    std::string name_;
    std::string buffer_;
};
} // namespace lua
} // namespace message
} // namespace robot
