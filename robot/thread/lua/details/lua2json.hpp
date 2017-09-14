#pragma once

#include <ostream>
#include <sol/sol.hpp>
#include <string>

namespace robot {
namespace thread {
namespace lua {
namespace details {
void serialize_table(sol::table value, std::ostream& os, std::string& buffer);
}
} // namespace lua
} // namespace thread
} // namespace robot