#include "serialize.hpp"
#include <message/trade/on_trans_reply.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {
template <>
void serialize<message::trade::on_trans_reply>(sol::table tab, message::trade::on_trans_reply& reply) {
}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot