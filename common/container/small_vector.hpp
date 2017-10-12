#pragma once

#include <boost/container/small_vector.hpp>

namespace common {
namespace container {

template <typename T, unsigned N>
using small_vector = boost::container::small_vector<T, N>;

} // namespace container
} // namespace common

