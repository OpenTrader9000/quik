#pragma once

#include "reader.hpp"
#include <common/storage/serialize.hpp>
#include <utils/filesystem/mapped_file.hpp>
#include <memory>

#include <zlib/zlib.h>

namespace persistent {
namespace trade {
namespace trade {



reader::reader(std::string const& path) {
    load_file_content(path);
}

reader::~reader() {}

void reader::load_file_content(std::string const& path) {
    utils::fs::mapped_file file(path.c_str());

    unsigned const place_for_header = common::storage::place_for_data<header>();
    unsigned uncompressed_size_position = place_for_header;
    unsigned data_position              = place_for_header + sizeof(unsigned);

    auto header_ptr = reinterpret_cast<header*>(file.data());
    if ((header_ptr->flags_ & (1 << header::COMPRESSED)) == 0) {
        throw std::runtime_error("File must be compressed");
    }

    unsigned uncompressed_size = *reinterpret_cast<unsigned*>(place_for_header + file.data());

    auto data = reinterpret_cast<unsigned char*>(file.data() + data_position);

    bulk_ptr_ = std::make_shared<persistent::trade::trade::bulk>();
    bulk_ptr_->bulk_storage_.resize(uncompressed_size);
    bulk_ptr_->elements_count_ = header_ptr->trades_count_;
    bulk_ptr_->sec_code_       = header_ptr->sec_code_;

    uLongf bulk_len = uncompressed_size;

    ::uncompress(bulk_ptr_->bulk_storage_.data(), &bulk_len, data,
                 common::numbers::integer_cast<uLong>(file.length() - data_position));
    assert(bulk_len == uncompressed_size);
}

void reader::read_all_trades(trades_sequence_t& buffer) {
    assert(bulk_ptr_);
    bulk_ptr_->walk_throught([&](common::storage::trade& value) { 
        buffer.push_back(value); 
    });
}

} // namespace trade
} // namespace trade
} // namespace persistent