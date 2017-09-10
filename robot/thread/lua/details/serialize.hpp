#pragma once

#include <common/message/message.hpp>
#include <sol/sol.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

template <typename ConcreteType>
void serialize(sol::table table, ConcreteType& result);

template <typename MessageType>
inline void deserialize(sol::table table, common::message::ptr_concrete<MessageType>& result) {
    serialize(table, *result);
}

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot