#include "quote.hpp"
#include <persistent/trade/quote/reader.hpp>
#include <persistent/trade/quote/writer.hpp>


namespace archtool {
namespace quote {
void process_compress(std::string const& path) {
    persistent::trade::quote::writer wr(path, "");
    wr.compress();
}

void process_file(std::string const& path, utils::cli_arguments const& args) {
    if (args.has_key("--compress")) {
        process_compress(path);
    }
}
} // namespace quote
} // namespace archtool