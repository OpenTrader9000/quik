#pragma once

#include <common/storage/trade.hpp>
#include <common/message/base.hpp>
#include <common/message/codes.hpp>

namespace common {
namespace message {
namespace trade {
struct trade : public ::common::message::base {

    enum { code = codes::TRADE };

    trade()
    : base(code)
    , trade_{} {}

    numbers::bcd& price() { return trade_.price_; }
    unsigned&     quantity() { return trade_.quantity_; }
    unsigned&     open_interest() { return trade_.open_interest_; }
    uint64_t&     machine_timestamp() { return trade_.machine_timestamp_; }
    uint64_t&     server_timestamp() { return trade_.server_timestamp_; }
	uint64_t&     trade_number() { return trade_.trade_num_; }
    
    std::string     sec_code_;
    storage::trade  trade_;
};
} // namespace trade
} // namespace message
} // namespace common