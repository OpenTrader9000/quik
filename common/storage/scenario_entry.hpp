#pragma once

#pragma once

#include <stdint.h>
#include <string>

namespace common {
namespace storage {

// type of the unhandled message
// 1 - any
// 2 - lua function
enum class scenario_entry_type { plain = 1, function = 2 };

struct scenario_entry {
    std::string name_;
    std::string body_;
    std::string info_;
    int64_t timestamp_;
    int64_t scenario_id_;
    int type_ = static_cast<int>(scenario_entry_type::plain);
};
} // namespace storage
} // namespace common
