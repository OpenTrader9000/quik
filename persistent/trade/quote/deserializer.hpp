#pragma once

#include "file.hpp"
#include <common/storage/trade.hpp>
#include <persistent/pch.hpp>

namespace persistent {
namespace trade {
namespace quote {

struct deserializer {

    using quote_t = common::storage::quote;

    std::string sec_code_;
    std::string storage_folder_;

    deserializer(std::string const& sec_code, std::string const& storage_folder);
    ~deserializer();

    std::vector<quote_t> deserialize(uint64_t start_time_in_ms,
                                     uint64_t end_time_in_ms,
                                     unsigned upper_limit = std::numeric_limits<unsigned>::max());
};
} // namespace quote
} // namespace trade
} // namespace persistent