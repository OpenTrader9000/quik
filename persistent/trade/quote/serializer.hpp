#pragma once

#include "file.hpp"
#include <persistent/pch.hpp>

namespace persistent {
namespace trade {
namespace quote {
struct serializer : public file {
    
    serializer(std::string const& sec_code, std::string const& storage_folder, uint64_t start_time);
    ~serializer();

    void serialize(messages_view_t messages, unsigned count);

private:
	uint64_t start_time_;

};
} // namespace trade
} // namespace trade
} // namespace persistent