#pragma once

#include <utils/process/cli_arguments.hpp>

namespace archtool {
namespace trade {
void process_file(std::string const& path, utils::cli_arguments const& args);
}
} // namespace archtool