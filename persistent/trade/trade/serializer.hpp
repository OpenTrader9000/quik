#pragma once

#include "file.hpp"
#include <persistent/pch.hpp>

namespace persistent {
namespace trade {
namespace trade {
struct serializer : public file {
    
    serializer(std::string const& sec_code, std::string const& storage_folder);
    ~serializer();

    void serialize(messages_view_t messages, unsigned count, uint64_t start_time, uint64_t end_time);

};
} // namespace trade
} // namespace trade
} // namespace persistent