#include "serialize.hpp"
#include <common/message/general/unhandled.hpp>
#include <utils/string/string_streambuf.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

using namespace common::message::general;

//\brief short function for a serialization
// \param tab - lua table
// \param os - ostream with string inside
// \param buffer - string from ostream
static void serialize_object(sol::table tab, std::ostream& os, std::string& buffer) {


    os << '{';
    tab.for_each([&](sol::object const& key, sol::object const& value) {

        sol::type key_type   = key.get_type();
        sol::type value_type = value.get_type();

        if (key_type == sol::type::number) {
            os << '\"' << key.as<int>() << '\"';
        } else if (key_type == sol::type::string) {
            os << '\"' << key.as<char const*>() << '\"';
        } else {
            assert(false);
        }

        os << " : ";

        if (value_type == sol::type::number) {
            // floating point values must be strings
            os << value.as<int64_t>();
        } else if (value_type == sol::type::string) {
            os << "\"" << value.as<char const*>() << "\"";
        } else if (value_type == sol::type::table) {
            serialize_object(value, os, buffer);
        } else if (value_type == sol::type::boolean) {
            os << value.as<bool>() ? "true" : "false";
        } else {
            os << "null";
        }
        os << ",";
    });

    if (buffer.back() == ',')
        buffer.pop_back();
    os << '}';
}

template <>
void serialize<unhandled>(sol::table tab, unhandled& reply) {

    reply.buffer_.resize(2048);
    utils::string_streambuf sb(reply.buffer_);
    std::ostream os(&sb);

    serialize_object(tab, os, reply.buffer_);
}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot