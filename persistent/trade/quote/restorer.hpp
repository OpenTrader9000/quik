#pragma once

#include <string>

namespace persistent {
namespace trade {
namespace quote {

// temprorary class for workaround quote error
// save previous quotes for sure
void restore(std::string const& path);

} // namespace quote
} // namespace trade
} // namespace persistent