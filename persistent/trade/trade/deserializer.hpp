#pragma once

#include "file.hpp"
#include <common/storage/trade.hpp>
#include <persistent/pch.hpp>

namespace persistent {
namespace trade {
namespace trade {
struct deserializer {

    using trade_t = common::storage::trade;

    std::string sec_code_;
    std::string storage_folder_;

    deserializer(std::string const& sec_code, std::string const& storage_folder);
    ~deserializer();

    std::vector<trade_t> deserialize(uint64_t start_time_in_ms,
                                     uint64_t end_time_in_ms,
                                     unsigned upper_limit = std::numeric_limits<unsigned>::max());
};
} // namespace trade
} // namespace trade
} // namespace persistent