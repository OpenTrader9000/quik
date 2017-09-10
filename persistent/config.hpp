#pragma once

#include <string>

namespace persistent {
    struct config {

        std::string path2db_;

        explicit operator bool() const {
            return !path2db_.empty();
        }
    };
}