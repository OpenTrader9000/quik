#pragma once

#include <unordered_set>

namespace common {
namespace container {

template <typename T>
using stack_set = std::unordered_set<T>;
}
} // namespace common