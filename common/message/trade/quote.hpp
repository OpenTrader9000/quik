#pragma once

#include <common/storage/quote.hpp>
#include <message/base.hpp>
#include <message/codes.hpp>

namespace common {
namespace message {
namespace trade {
struct quote : public ::common::message::base {

    enum { code = codes::QUOTE };

    quote()
    : base(code)
    , quote_{} {}

    numbers::bcd& price() { return quote_.price_; }
    int&          quantity_diff() { return quote_.quantity_diff_; }
    uint64_t&     machine_timestamp() { return quote_.machine_timestamp_; }

    std::string    sec_code_;
    storage::quote quote_;
};
} // namespace trade
} // namespace message
} // namespace common