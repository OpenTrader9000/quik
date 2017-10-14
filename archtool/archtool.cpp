// archtool.cpp : Defines the entry point for the console application.
//

#include <utils/process/cli_arguments.hpp>
#include <utils/filesystem/filesystem.hpp>
#include <iostream>

#include "trade/trade.hpp"

int main(int argc, char* argv[]) {

    utils::cli_arguments args(argc, argv);

    if (!args.has_key("--file")) {
        std::cerr << "Provide any file for input (--file) \n";
        return 0;
    }

    std::string filename = args.value("--file");
    if (filename.empty())
        return 0;

    std::string extension = utils::fs::extension(filename);
    if (extension.compare("trd") == 0) {
        archtool::trade::process_file(filename, args);
    }


    return 0;
}
