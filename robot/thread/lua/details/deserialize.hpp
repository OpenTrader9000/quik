#pragma once

#include <message/base.hpp>
#include <sol/sol.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

template <typename ConcreteType>
void deserialize(sol::table table, ConcreteType& result);

template <typename MessageType>
inline void deserialize(sol::table table, message::ptr_concrete<MessageType>& result) {
    deserialize(table, *result);
}

} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot