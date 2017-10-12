#pragma once

#include <common/container/small_vector.hpp>
#include <common/storage/quote.hpp>
#include <common/message/base.hpp>
#include <common/message/codes.hpp>


namespace common {
namespace message {
namespace trade {
struct quotes : public ::common::message::base {

    using storage_t = common::container::small_vector<storage::quote, 8>;

    enum { code = codes::QUOTES };

    quotes()
    : base(code) {}

    std::string sec_code_;
    storage_t   quotes_;
};
} // namespace trade
} // namespace message
} // namespace common