#pragma once

#include <utils/process/cli_arguments.hpp>

namespace archtool {
namespace trade {
// possible :
// --format csv --file "C:\Users\Andrey\20170920.trd"  --output out.txt
void process_file(std::string const& path, utils::cli_arguments const& args);
}
} // namespace archtool