#pragma once


#include "file.hpp"

#include <string>
#include <memory>
#include <functional>
#include <limits>

#include <common/storage/compress/compress.hpp>

namespace persistent {
namespace trade {
namespace quote {

struct reader;

struct bulk_state {
    // end of buffer will be maximum value of position
    size_t                     position_in_buffer_ = 0;
    common::storage::quote     quote_ = {};

    bulk_state(){}
    bulk_state(size_t position, common::storage::quote q)
        : position_in_buffer_(position)
        , quote_(q) {}

    bool is_end_reached() const {
        return position_in_buffer_ == std::numeric_limits<size_t>::max();
    }
};

// cached storage for quotes
struct bulk {

    using quote_t = common::storage::quote;

    bulk();
    ~bulk();
    
    template<typename FunctionType>
    void walk_throught(FunctionType&& callback) const;

    uint64_t           elements_count() const { return elements_count_; }
    std::string const& sec_code() const { return sec_code_; }
    size_t             data_size() const { return bulk_storage_.size(); }

    // read quotes till the and or callback returns error
    // allways return value. In the end position and quote of the state assign to start values
    // for init bulk_state run this function with callback that returns false
    // 
    //  inited_state = bulk_ptr->walk(inited_state, [](quote_t const&) { return false; });
    bulk_state walk(bulk_state state, std::function<bool(quote_t const&)> callback) const;
    
 private:
    
    friend struct reader;

    // stores integers in utf-8 similar data type
    std::vector<unsigned char> bulk_storage_;
    uint64_t                   elements_count_;
    std::string                sec_code_;
};

// bulk must be a shared object
using bulk_ptr_t = std::shared_ptr<bulk>;

template <typename FunctionType>
inline void bulk::walk_throught(FunctionType&& callback) const {
    auto compressor = common::storage::quote::make_compressor();

    unsigned char const* begin = bulk_storage_.data();
    unsigned char const* end   = begin + bulk_storage_.size();
    unsigned             size  = static_cast<unsigned>(bulk_storage_.size());

    while (begin != end) {
        common::storage::quote value;
        common::storage::compress::decode(value, compressor, begin, size);
        callback(value);
    }
}

} // namespace quote
} // namespace trade
} // namespace persistent