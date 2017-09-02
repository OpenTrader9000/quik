#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace common {
namespace message {
namespace general {
struct unhandled : public base {
    enum { code = codes::UNHANDLED };

    unhandled()
    : base(code) {
    }

    std::string name_;
    std::string buffer_;
};

} // namespace general
} // namespace message
} // namespace common
