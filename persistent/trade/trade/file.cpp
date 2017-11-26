#include "file.hpp"

#include <iomanip>
#include <time.h>
#include <utils/string/build.hpp>


namespace persistent {
namespace trade {
namespace trade {
std::string path(std::string const& sec_code, std::string const& storage_folder, uint64_t timestamp) {
    // compute time
    time_t    t = static_cast<time_t>(timestamp) / 1000;
    struct tm time;
    ::localtime_s(&time, &t);

    // sec_code/year/month/dd.trd
    // RIU7/2017/10/07.trd
    return utils::build_string(storage_folder, 
                              '/', 1900 + time.tm_year, 
                              '/', sec_code, 
                              '/', 1900 + time.tm_year, std::setw(2), std::setfill('0'), 1 + time.tm_mon,
                               std::setw(2), std::setfill('0'), time.tm_mday, ".trd");
}
} // namespace trade
} // namespace trade
} // namespace persistent