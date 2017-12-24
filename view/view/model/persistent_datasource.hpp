#pragma once

#include "datasource.hpp"
#include <persistent/trade/symbol_storage.hpp>
#include <unordered_set>
#include <unordered_map>


namespace view {
namespace model {


struct persistent_datasource : public datasource {

    using series_t = persistent::trade::series;
    using sym_cache_t =
        std::unordered_map<series_key_t, persistent::trade::series, series_key_t::hash, series_key_t::equals>;
    using storage_t = std::unordered_map<std::string, persistent::trade::symbol_storage>;

    persistent_datasource(std::string const& path2archive);
    ~persistent_datasource();

    virtual ohlc_view_t get_elements(series_key_t key,
                                     unsigned start_index, unsigned count) override;

    virtual ohlc_view_t get_elements_starts_with_index(series_key_t key,
                                                       unsigned start_index) override;

    virtual order_book_t get_order_book(std::string const& sec_code, uint64_t ts) override;

    virtual strings_view_t symbols() override;

    virtual size_t count_of_elements(series_key_t key) override;

 private:

    series_t const& series(series_key_t key);

    std::string              path_to_storage_;
    std::vector<std::string> list_of_symbols_;
    storage_t                storage_;
    sym_cache_t              cache_;
};
} // namespace model
} // namespace view