#include "deserializer.hpp"
#include <algorithm>
#include <iterator>


namespace persistent {
namespace trade {
namespace trade {
deserializer::deserializer(std::string const& sec_code, std::string const& storage_folder)
: sec_code_(sec_code)
, storage_folder_(storage_folder) {}

deserializer::~deserializer() {}

std::vector<deserializer::trade_t>
deserializer::deserialize(uint64_t start_time_in_ms, uint64_t end_time_in_ms, unsigned upper_limit) {
   
    constexpr uint64_t day_time = 24 * 60 * 60;

    std::vector<trade_t> result;

    uint64_t start_time           = start_time_in_ms / 1000;
    uint64_t end_time             = end_time_in_ms / 1000;
    uint64_t possible_files_count = (end_time - start_time) / day_time + 1;

    std::vector<file> files;
    files.reserve(possible_files_count);

    // open and count items
    unsigned trades_count = 0;
    unsigned max_trades_count = 0;
    for (unsigned idx = 0; idx < possible_files_count; ++idx) {
        files.emplace_back(sec_code_, storage_folder_, file::mode::READ);

        uint64_t time = start_time + day_time * idx;
        
        file& last = files.back();
        if (!last.open(time)) {
            files.pop_back();
            continue;
        }

        // might be some troubles with this algorithm
        trades_count += last.header_.trades_count_; // TODO: make here const)
        max_trades_count = std::max(last.header_.trades_count_, max_trades_count);
        if (trades_count >= upper_limit)
            break;        
    }

    result.reserve(upper_limit > trades_count ? trades_count : upper_limit);
    
    //std::vector<trade_t> buffer;
    //buffer.reserve(max_trades_count);
    for (auto& f : files) {
        f.read_trades(result);
    }

    return result;
}
} // namespace trade
} // namespace trade
} // namespace persistent