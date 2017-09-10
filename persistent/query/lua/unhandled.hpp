#pragma once
#include <stdint.h>

namespace persistent {
namespace query {
namespace lua {


void insert_unhandled(int64_t timestamp, char const* name, char const* src);
}
} // namespace query
} // namespace persistent