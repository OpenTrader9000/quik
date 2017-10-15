#include "deserializer.hpp"
#include <algorithm>
#include <iterator>


namespace persistent {
namespace trade {
namespace quote {

deserializer::deserializer(std::string const& sec_code, std::string const& storage_folder)
: sec_code_(sec_code)
, storage_folder_(storage_folder) {}

deserializer::~deserializer() {}

std::vector<deserializer::quote_t>
deserializer::deserialize(uint64_t start_time_in_ms, uint64_t end_time_in_ms, unsigned upper_limit) {
   
    constexpr uint64_t day_time = 24 * 60 * 60;

    std::vector<quote_t> result;

    uint64_t start_time           = start_time_in_ms / 1000;
    uint64_t end_time             = end_time_in_ms / 1000;
    uint64_t possible_files_count = (end_time - start_time) / day_time + 1;

    std::vector<file> files;
    files.reserve(possible_files_count);

    // open and count items
    uint64_t quotes_count = 0;
    uint64_t max_quotes_count = 0;
    for (unsigned idx = 0; idx < possible_files_count; ++idx) {
        uint64_t time = start_time + day_time * idx;

        files.emplace_back(sec_code_, storage_folder_, time, file::mode::READ);
        file& last = files.back();
        if (!last.open()) {
            files.pop_back();
            continue;
        }

        uint64_t file_quotes_count = last.file_header().quotes_count_;

        // might be some troubles with this algorithm
        quotes_count += file_quotes_count; // TODO: make here const)
        max_quotes_count = std::max(file_quotes_count, max_quotes_count);
        if (quotes_count >= upper_limit)
            break;        
    }

    result.reserve(upper_limit > quotes_count ? quotes_count : upper_limit);
    
    //std::vector<trade_t> buffer;
    //buffer.reserve(max_quotes_count);
    for (auto& f : files) {
        f.read_quotes(result);
    }

    return result;
}
} // namespace trade
} // namespace trade
} // namespace persistent