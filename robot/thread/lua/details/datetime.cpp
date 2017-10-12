#include "datetime.hpp"

namespace robot {
namespace thread {
namespace lua {
namespace details {
uint64_t timestamp(sol::table datetime) {

    uint64_t result;

    struct tm time {};

    time.tm_sec  = datetime["seconds"];
    time.tm_min  = datetime["minutes"];
    time.tm_hour = datetime["hour"];
    time.tm_mday = datetime["day"];
    time.tm_mon  = datetime["month"];
    time.tm_year = datetime["year"];

    time.tm_mon -= 1;
    time.tm_year -= 1900;
    time.tm_min += 1;

    unsigned ms = datetime["ms"];

    result = static_cast<uint64_t>(mktime(&time));

    return result * 1000 + ms;
}
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot