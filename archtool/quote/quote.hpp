#pragma once

#include <utils/process/cli_arguments.hpp>

namespace archtool {
namespace quote {
// possible :
// --compress --file "C:\Users\Andrey\20170920.ob"  
void process_file(std::string const& path, utils::cli_arguments const& args);
} // namespace quote
} // namespace archtool