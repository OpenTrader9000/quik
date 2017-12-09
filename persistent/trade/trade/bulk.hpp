#pragma once


#include "file.hpp"

#include <memory>
#include <string>

#include <common/storage/compress/compress.hpp>

namespace persistent {
namespace trade {
namespace trade {

struct reader;

struct bulk_position {
    unsigned               position_in_buffer_ = 0;
    common::storage::trade trade_              = {};
};

// cached storage for quotes
struct bulk {

    bulk();
    ~bulk();

    template <typename FunctionType>
    void walk_throught(FunctionType&& callback);

    uint64_t           elements_count() const { return elements_count_; }
    std::string const& sec_code() const { return sec_code_; }
    size_t             data_size() const { return bulk_storage_.size(); }

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
inline void bulk::walk_throught(FunctionType&& callback) {
    auto compressor = common::storage::trade::make_compressor();

    unsigned char const* begin = bulk_storage_.data();
    unsigned char const* end   = begin + bulk_storage_.size();
    unsigned             size  = static_cast<unsigned>(bulk_storage_.size());

    while (begin != end) {
        common::storage::trade value;
        common::storage::compress::decode(value, compressor, begin, size);
        callback(value);
    }
}

} // namespace quote
} // namespace trade
} // namespace persistent