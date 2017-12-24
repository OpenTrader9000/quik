#include "deserialize.hpp"
#include "lua2json.hpp"
#include <common/message/scenario/entry.hpp>
#include <utils/string/string_streambuf.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

// most the same code as unhandled message

using namespace common::message::scenario;

template <>
void deserialize<entry>(sol::table tab, entry& reply) {

    reply.body().reserve(2048);
    utils::string_streambuf sb(reply.body());
    std::ostream os(&sb);

    // disable scientific notation
    os.setf(std::ios::fixed, std::ios::floatfield);
    os.setf(std::ios::showpoint);


    serialize_table(tab, os, reply.body());
}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot