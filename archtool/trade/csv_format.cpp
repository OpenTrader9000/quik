#include "csv_format.hpp"
#include <utils/string/build.hpp>
#include <utils/string/string.hpp>
#include <unordered_map>
#include <functional>
#include <time.h>
#include <fstream>
#include <iomanip>
#include <iostream>

namespace archtool {
namespace trade {

using trade_t = common::storage::trade;

struct tm create_time(uint64_t time_in_ms) {
    time_t    t = static_cast<time_t>(time_in_ms/1000);
    struct tm time;
    ::localtime_s(&time, &t);

    return time;
}

std::ostream& YMD(uint64_t time_in_ms, std::ostream& out) {
    auto t = create_time(time_in_ms);
    out << 1900 + t.tm_year << std::setw(2) << std::setfill('0') << 1 + t.tm_mon << std::setw(2)
        << std::setfill('0') << t.tm_mday;
    return out;
}

std::ostream& TIME(uint64_t time_in_ms, std::ostream& out) {
    auto t = create_time(time_in_ms);
    out << std::setw(2) << std::setfill('0') << t.tm_hour
        << std::setw(2) << std::setfill('0') << t.tm_min
        << std::setw(2) << std::setfill('0') << t.tm_sec
        ;
    return out;
}


std::unordered_map<std::string, std::function<void(trade_t const&, std::ostream&)>> serializers = {
    { "PRICE", [](trade_t const& trade, std::ostream& out) { out << trade.price_.to_string(); } },
    { "TRADE_NUM", [](trade_t const& trade, std::ostream& out) { out << trade.trade_num_; }},
    { "QUANTITY", [](trade_t const& trade, std::ostream& out) { out << trade.quantity_; } },
    { "YMD", [](trade_t const& trade, std::ostream& out) { YMD(trade.server_timestamp_, out); } },
    { "TIME", [](trade_t const& trade, std::ostream& out) { TIME(trade.server_timestamp_, out); } },
    { "MS", [](trade_t const& trade, std::ostream& out) { out << trade.server_timestamp_%1000; } },
    { "YMD_LOCAL", [](trade_t const& trade, std::ostream& out) { YMD(trade.machine_timestamp_, out); } },
    { "TIME_LOCAL", [](trade_t const& trade, std::ostream& out) { TIME(trade.machine_timestamp_, out); } },
    { "MS_LOCAL", [](trade_t const& trade, std::ostream& out) { out << trade.machine_timestamp_ % 1000; } },
    { "DIRECTION", [](trade_t const& trade, std::ostream& out) { out << (trade.is_bid() ? "B" : "S"); }},
};


csv_format::csv_format(utils::cli_arguments const& arguments) {
    if (arguments.has_key("--fields")) {
        fields_ = utils::split(arguments.value("--fields"), ',');
    }
    else {
        fields_ = { "TRADE_NUM", "YMD", "TIME", "PRICE", "QUANTITY" };
    }
    buffer_.reserve(1000000); // this is might be very small amount of size
}

csv_format::~csv_format() {}

void        csv_format::add(common::storage::trade const& trade) {
    utils::string_streambuf sb(buffer_);
    std::ostream            output(&sb);

    for (std::string const& key : fields_) {
        serializers.at(key)(trade, output);
        output << ",";
    }
    output.flush();
    buffer_.back() = '\n';
}

void        csv_format::flush(std::string const& path) {

    if (path.empty()) {
        std::cout << buffer_;
        return;
    }

    std::ofstream out(path);
    out << buffer_;
}

std::string csv_format::help() {
    std::string result;
    utils::string_streambuf sb(result);
    std::ostream            output(&sb);

    output << "csv trades formatter\n"
           << "\t--fields  comma separated fields names for output\n";
    output << "possible value of fields ";
    for (auto& fmt : serializers) {
        output << fmt.first << ",";
    }
    output.flush();
    result.back() = '\n';
    return result;
}
} // namespace trade
} // namespace archtool