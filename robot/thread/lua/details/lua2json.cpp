#include "lua2json.hpp"


namespace robot {
namespace thread {
namespace lua {
namespace details {

static void serialize_array(sol::table tab, std::ostream& os, std::string& buffer);
static void serialize_object(sol::table tab, std::ostream& os, std::string& buffer);

static void serialize_key(sol::object const& key, std::ostream& os, std::string& buffer) {
    sol::type key_type = key.get_type();

    if (key_type == sol::type::string) {
        os << '\"' << key.as<char const*>() << '\"';
    } else {
        assert(false);
    }
}

static void serialize_value(sol::object const& value, std::ostream& os, std::string& buffer) {
    sol::type value_type = value.get_type();

    if (value_type == sol::type::number) {
        // floating point values must be strings
        os << value.as<double>();
    } else if (value_type == sol::type::string) {
        os << " \"" << value.as<char const*>() << "\"";
    } else if (value_type == sol::type::table) {
        if (luaL_getn(value.lua_state(), -1) == 0)
            serialize_object(value, os, buffer);
        else
            serialize_array(value, os, buffer);
    } else if (value_type == sol::type::boolean) {
        os << value.as<bool>() ? "true" : "false";
    } else {
        os << "null";
    }
}

//\brief function for serialization of the array
// \param tab - lua table
// \param os - stream for serialization
// \param buffer - string
static void serialize_array(sol::table tab, std::ostream& os, std::string& buffer) {

    os << '[';
    tab.for_each([&](sol::object const&, sol::object const& value) {
        serialize_value(value, os, buffer);
        os << ",";
    });

    if (buffer.back() == ',')
        buffer.pop_back();

    os << ']';
}

//\brief short function for a serialization
// \param tab - lua table
// \param os - ostream with string inside
// \param buffer - string from ostream (for removing dot =)) )
static void serialize_object(sol::table tab, std::ostream& os, std::string& buffer) {

    os << '{';
    tab.for_each([&](sol::object const& key, sol::object const& value) {
        serialize_key(key, os, buffer);
        os << " : ";
        serialize_value(value, os, buffer);
        os << ",";
    });

    if (buffer.back() == ',')
        buffer.pop_back();

    os << '}';
}

void serialize_table(sol::table value, std::ostream& os, std::string& buffer) {
    serialize_object(value, os, buffer);
}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot