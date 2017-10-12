#pragma once

#include <sol/sol.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {
uint64_t timestamp(sol::table datetime);
}
} // namespace lua
} // namespace thread
} // namespace robot