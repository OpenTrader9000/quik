#include "deserialize.hpp"
#include <common/message/general/unhandled.hpp>
#include <utils/string/string_streambuf.hpp>
#include "lua2json.hpp"

namespace robot {
namespace thread {
namespace lua {
namespace details {

using namespace common::message::general;

template <>
void deserialize<unhandled>(sol::table tab, unhandled& reply) {

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