// archtool.cpp : Defines the entry point for the console application.
//

#include <iostream>
#include <utils/filesystem/filesystem.hpp>
#include <utils/process/cli_arguments.hpp>

#include "trade/trade.hpp"
#include "quote/quote.hpp"

int main(int argc, char* argv[]) {

    utils::cli_arguments args(argc, argv);

    bool has_file_key = args.has_key("--file");

    if (!has_file_key) {
        std::cerr << "Provide any file for input (--file/--folder) \n";
        return 0;
    }

    std::string filename = args.value("--file");
    if (filename.empty())
        return 0;

    std::string extension = utils::fs::extension(filename);
    if (extension.compare("trd") == 0) {
        archtool::trade::process_file(filename, args);
    }
    else if (extension.compare("ob") == 0) {
        archtool::quote::process_file(filename, args);
    }

    return 0;
}
