#pragma once

#include <common/numbers/bcd.hpp>
#include <tuple>
#include <common/storage/compress/make.hpp>


namespace common {
namespace storage {
struct quote {
    
    enum {
        BID = 0 // flag position
    };

    void set_bid() { flags_ |= (1 << BID); }
    void set_offer() { flags_ &= ~(1 << BID); }

    bool is_bid() const { return (flags_ & (1 << BID)) != 0; }
    bool is_offer() const { return (flags_ & (1 << BID)) == 0; }

    static constexpr auto fields() {
        return std::make_tuple(&quote::flags_, &quote::price_, &quote::quantity_diff_, &quote::machine_timestamp_);
    }

    static auto make_compressor() {
        using namespace common::storage::compress;
        return std::make_tuple(compressor(&quote::flags_), history_compressor(&quote::price_),
                               compressor(&quote::quantity_diff_),
                               history_compressor(&quote::machine_timestamp_));
    }

    unsigned char flags_;
    numbers::bcd  price_;
    int           quantity_diff_;
    uint64_t      machine_timestamp_;
};
} // namespace storage
} // namespace common