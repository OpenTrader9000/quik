#pragma once

#include <string>
#include <common/storage/trade.hpp>

namespace archtool {
namespace trade {
// format buffer
struct format {
    virtual void        add(common::storage::trade const& trade) = 0;
    virtual void        flush(std::string const& path)           = 0;
    virtual std::string help()                                   = 0;
};
} // namespace trade
} // namespace archtool