#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>
#include <common/storage/unhandled.hpp>

namespace common {
namespace message {
namespace general {
struct unhandled : public base {

    enum { code = codes::UNHANDLED };

    unhandled()
    : base(code) {
    }

    storage::unhandled body_;

    std::string& name()     { return body_.name_; }
    std::string& body()     { return body_.body_; }
    uint64_t    & timestamp(){ return body_.timestamp_; }

    void set_type_plain()        { body_.type_ = static_cast<int>(storage::unhandled_message_type::plain); }
    void set_type_function()     { body_.type_ = static_cast<int>(storage::unhandled_message_type::function); }
};

} // namespace general
} // namespace message
} // namespace common
