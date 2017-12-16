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
    void walk_throught(FunctionType&& callback) const;

    // probably this function will have not been called
    template <typename FunctionType>
    void walk_period(uint64_t start_ts, uint64_t end_ts, FunctionType&& callback) const;

    template <typename StartFunctionType, typename UpdateFunctionType, typename EndFunctionType>
    void walk_period_by_interval(uint64_t start_ts, uint64_t end_ts, uint64_t shift,
                                 uint64_t interval, StartFunctionType&& start_f,
                                 UpdateFunctionType&& update_f, EndFunctionType&& end) const;


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
inline void bulk::walk_throught(FunctionType&& callback) const {
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

template <typename FunctionType>
inline void bulk::walk_period(uint64_t start_ts, uint64_t end_ts, FunctionType&& callback) const {
    auto compressor = common::storage::trade::make_compressor();

    unsigned char const* begin = bulk_storage_.data();
    unsigned char const* end   = begin + bulk_storage_.size();
    unsigned             size  = static_cast<unsigned>(bulk_storage_.size());

    // skip elements under low range
    common::storage::trade value = {};
    while (begin != end && value.server_timestamp_ < start_ts) {
        common::storage::compress::decode(value, compressor, begin, size);
    }

    while (begin != end && value.server_timestamp_ < end_ts) {
        common::storage::compress::decode(value, compressor, begin, size);
        callback(value);
    }
}

template <typename StartFunctionType, typename UpdateFunctionType, typename EndFunctionType>
inline void bulk::walk_period_by_interval(uint64_t start_ts, uint64_t end_ts, uint64_t shift,
                                          uint64_t interval_ts, StartFunctionType&& start_f,
                                          UpdateFunctionType&& update_f, EndFunctionType&& end_f) const {

    // TODO: here must be function with structured binding when code base will have migrated to c++17
    auto compressor = common::storage::trade::make_compressor();

    unsigned char const* begin = bulk_storage_.data();
    unsigned char const* end = begin + bulk_storage_.size();
    unsigned             size = static_cast<unsigned>(bulk_storage_.size());

    // shugar for next value
    auto next = [&]() {
        common::storage::trade t{};
        if (begin != end) { // LIKELY
            common::storage::compress::decode(t, compressor, begin, size);
        } 
        return t;
    };



    // two value for a proper detection end and start of period
    common::storage::trade previous_value = {}, next_value = {};

    uint64_t interval_start_ts = (start_ts - start_ts % interval_ts) + shift;

    // initial state of both values must be place after start of interval
    while (begin != end && previous_value.server_timestamp_ < interval_start_ts) {
        previous_value = next_value;
        next_value = next();
    }

    // align start timestamp
    uint64_t unaligned_start_ts = previous_value.server_timestamp_;

    // for the case when values doesn't got into the interval
    interval_start_ts = (unaligned_start_ts - unaligned_start_ts % interval_ts) + shift;
    uint64_t interval_end_ts = interval_start_ts + interval_ts;


    while (begin != end && previous_value.server_timestamp_ < end_ts && next_value.server_timestamp_ != 0) {

        // previous_value is the first value in interval
        start_f(previous_value);
        

        // server_timestamp == 0 is cancelation token
        while (begin != end && next_value.server_timestamp_ < interval_end_ts &&
               next_value.server_timestamp_ != 0) {
            
            update_f(next_value);

            previous_value = next_value;
            next_value = next();
        }

        // previous_value is the last value in interval
        end_f(previous_value);

        // previous_value is the first value in new interval
        previous_value = next_value;
        next_value = next();

        interval_start_ts += interval_ts;
        interval_end_ts += interval_ts;
    }

}

} // namespace quote
} // namespace trade
} // namespace persistent