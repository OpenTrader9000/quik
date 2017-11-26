#include "trade.hpp"

#include <utils/string/build.hpp>
#include <persistent/trade/trade/reader.hpp>
#include <iostream>
#include <memory>

#include "format.hpp"
#include "csv_format.hpp"

namespace archtool {
namespace trade {

std::shared_ptr<format> create(std::string const& name, utils::cli_arguments const& args) {
    if (name.compare("csv") == 0) {
        return std::make_shared<csv_format>(args);
    }

    throw std::runtime_error(utils::build_string("Unknown format ", name));

    return nullptr;
}

void process_file(std::string const& path, utils::cli_arguments const& args) {

    using namespace persistent::trade::trade;

    reader file(path);

    if (!args.has_key("--format"))
        return;

    std::string format = args.value("--format");
    //std::string options = args.value("--options");
    std::string output = args.value("--output");

    auto fmt = create(format, args);

    if (args.has_key("--help")) {
        std::cerr << fmt->help() << "\n";
        return;
    }
    
    trades_sequence_t storage;
    file.read_all_trades(storage);

    for (auto& trade : storage) {
        fmt->add(trade);
    }
    fmt->flush(output);
    
    auto const& bulk_ptr = file.bulk();

    std::cerr << "File " << path << "\n"
              << "sec code: " << bulk_ptr->sec_code() << "\n"
              << "trades count: " << bulk_ptr->elements_count() << "\n";
}
} // namespace trade
} // namespace archtool