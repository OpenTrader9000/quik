#pragma once

#include <stdint.h>
#include <tuple>
#include <common/storage/trade.hpp>
#include <vector>

namespace persistent {
namespace trade {
namespace trade {

struct header {
    // binary flags for a header
    enum FLAGS { COMPRESSED = 0 };

    char          sec_code_[16];
    uint64_t      trades_count_;
    unsigned      version_;
    unsigned char flags_;
    unsigned char reserved_[16]; // possible structure information

    static auto fields() {
        return std::make_tuple(&header::sec_code_, &header::trades_count_, &header::version_,
                               &header::flags_, &header::reserved_);
    }
};

std::string path(std::string const& sec_code, std::string const& storage_folder, uint64_t timestamp);

using trades_sequence_t = std::vector<common::storage::trade>;


} // namespace trade
} // namespace trade
} // namespace persistent