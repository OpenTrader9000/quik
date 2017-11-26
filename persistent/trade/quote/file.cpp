#pragma once
#include "file.hpp"
#include <time.h>
#include <utils/string/build.hpp>
#include <iomanip>

namespace persistent {
namespace trade {
namespace quote {
std::string path(std::string const& sec_code, std::string const& storage_folder, uint64_t timestamp) {

    constexpr uint64_t moscow_ts_diff_in_ms = 3 * 60 * 60 * 1000;

    // compute time
    time_t    t = static_cast<time_t>(timestamp - moscow_ts_diff_in_ms)/1000;
    struct tm time;
    ::localtime_s(&time, &t);

    // sec_code/year/month/dd.ob
    // RIU7/2017/10/07.ob
    return utils::build_string(storage_folder,
                               '/', time.tm_year + 1900,
                               '/', sec_code,
                               '/', time.tm_year + 1900,
                               std::setw(2), std::setfill('0'), time.tm_mon + 1,
                               std::setw(2), std::setfill('0'), time.tm_mday,
                               ".ob");
    }

}
} // namespace trade
} // namespace persistent


