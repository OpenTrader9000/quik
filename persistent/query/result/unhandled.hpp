#pragma once

#include <string>

namespace persistent {
namespace query {
namespace result {
struct unhandled {
    int64_t id_;
    std::string function_name_;
    int64_t ts_;
    std::string invoke_dump_;
};
} // namespace result
} // namespace query
} // namespace persistent
