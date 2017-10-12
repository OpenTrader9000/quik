#pragma once

#include <common/numbers/bcd.hpp>

namespace common {
namespace storage {
struct trade {

    enum {
        BID = 0 // flag position
    };

    void set_bid()   { flags_ |= (1 << BID); }
    void set_offer() { flags_ &= ~(1 << BID); }

    bool is_bid() const { return (flags_ & BID) != 0; }
    bool is_offer() const { return (flags_ & BID) == 0; }

    unsigned char flags_;
    numbers::bcd price_;
    uint64_t     trade_num_;
	uint64_t     machine_timestamp_;
	uint64_t     server_timestamp_;
    unsigned     quantity_;
    unsigned     open_interest_;
};
} // namespace storage
} // namespace common