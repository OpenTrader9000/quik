#pragma once

#include <common/numbers/bcd.hpp>
#include <tuple>
#include <common/storage/compress/make.hpp>


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

	static constexpr auto fields() {
        return std::make_tuple(&trade::flags_, &trade::price_, &trade::trade_num_, &trade::machine_timestamp_,
                               &trade::server_timestamp_, &trade::quantity_, &trade::open_interest_);
    }

    static auto make_compressor() {
        using namespace common::storage::compress;
        return std::make_tuple(
                        compressor(&trade::flags_),
                        history_compressor(&trade::price_),
                        history_compressor(&trade::trade_num_),
                        history_compressor(&trade::machine_timestamp_),
                        history_compressor(&trade::server_timestamp_),
                        compressor(&trade::quantity_),
                        history_compressor(&trade::open_interest_));
    }
};
} // namespace storage
} // namespace common