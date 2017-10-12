#pragma once

#include <common/numbers/bcd.hpp>

namespace common {
namespace storage {
struct quote {
    
    enum {
        BID = 0 // flag position
    };

    void set_bid() { flags_ |= (1 << BID); }
    void set_offer() { flags_ &= ~(1 << BID); }

    bool is_bid() const { return (flags_ & BID) != 0; }
    bool is_offer() const { return (flags_ & BID) == 0; }


    unsigned char flags_;
    numbers::bcd  price_;
    int           quantity_diff_;
    uint64_t      machine_timestamp_;
};
} // namespace storage
} // namespace common