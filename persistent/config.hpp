#pragma once

#include <string>

namespace persistent {
    struct config {

        std::string path2db_;
        std::string trades_archive_folder_;

        explicit operator bool() const {
            return !path2db_.empty();
        }
    };
}