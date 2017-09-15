


#pragma once

#include <common/message/base.hpp>
#include <common/message/codes.hpp>
#include <common/storage/scenario_entry.hpp>

namespace common {
namespace message {
namespace scenario {
struct entry : public base {

    enum { code = codes::SCENARIO_ENTRY };

    entry()
    : base(code) {}

    storage::scenario_entry body_;

    std::string& name() { return body_.name_; }
    std::string& body() { return body_.body_; }
    std::string& info() { return body_.info_; }
    int64_t& timestamp() { return body_.timestamp_; }
    int64_t& scenario_id() { return body_.scenario_id_; }
    void set_type_plain() { body_.type_ = static_cast<int>(storage::scenario_entry_type::plain); }
    void set_type_function() { body_.type_ = static_cast<int>(storage::scenario_entry_type::function); }
};

} // namespace scenario
} // namespace message
} // namespace common
