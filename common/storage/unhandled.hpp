#pragma once

#include <stdint.h>
#include <string>

namespace common {
namespace storage {

// type of the unhandled message
// 1 - any
// 2 - lua function
enum unhandled_message_type { plain = 1, function = 2 };

struct unhandled {
    int     index_;
    std::string name_;
    std::string body_;
    uint64_t timestamp_;
    int type_ = plain;
};
} // namespace storage
} // namespace common