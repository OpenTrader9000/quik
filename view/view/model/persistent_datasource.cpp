#include "persistent_datasource.hpp"
#include <utils/filesystem/filesystem.hpp>

namespace view {
namespace model {

persistent_datasource::persistent_datasource(std::string const& path2archive)
    : datasource(ds_flags::all)
    , path_to_storage_(path2archive) {
    list_of_symbols_ = utils::fs::subdirectories(path2archive + R"(\*)");
    std::sort(list_of_symbols_.begin(), list_of_symbols_.end());
}

persistent_datasource::~persistent_datasource() {}

ohlc_view_t persistent_datasource::get_elements(series_key_t key, unsigned start_index, unsigned count) {
    auto& ser = series(key).series_;
    if (ser.size() < start_index)
        return ohlc_view_t();
    
    size_t tail_size = ser.size() - start_index;
    return ohlc_view_t(ser.data() + start_index, (tail_size > count ? count : tail_size)); 
}
ohlc_view_t persistent_datasource::get_elements_starts_with_index(series_key_t key, unsigned start_index) {
    auto& ser = series(key).series_;
    if (ser.size() < start_index)
        return ohlc_view_t();

    size_t tail_size = ser.size() - start_index;
    return ohlc_view_t(ser.data() + start_index, tail_size);
}

order_book_t persistent_datasource::get_order_book(std::string const& sec_code, uint64_t ts) {
    // storage must be created at this very moment
    return storage_.at(sec_code).extract_order_book_by_timestamp(ts).order_book_;
}

strings_view_t persistent_datasource::symbols() {
    return list_of_symbols_;
}

size_t persistent_datasource::count_of_elements(series_key_t key) {
    return series(key).series_.size();
}
 
persistent_datasource::series_t const & persistent_datasource::series(series_key_t key){

    auto series_it = cache_.find(key);
    if (series_it != cache_.end())
        return series_it->second;

    // trying to find in symbol storage
    auto storage_it = storage_.find(key.sec_code_);
    if (storage_it != storage_.end()) {
        auto& symbol_storage = storage_it->second;
        cache_[key] = symbol_storage.extract(0, persistent::trade::symbol_storage::year2286, key.period_, key.shift_);
        return cache_[key];
    }

    // search throught all possible codes
    auto symbol_it = std::lower_bound(list_of_symbols_.begin(), list_of_symbols_.end(), key.sec_code_);
    if (symbol_it == list_of_symbols_.end())
        return cache_[series_key_t::make_empty()]; // return fake code;

    // create new symbol storage
    storage_.emplace(std::make_pair(key.sec_code_, persistent::trade::symbol_storage(path_to_storage_, key.sec_code_)));
    cache_[key] = storage_.at(key.sec_code_).extract(0, persistent::trade::symbol_storage::year2286, key.period_, key.shift_);
    return cache_[key];
}

} // namespace model
} // namespace view