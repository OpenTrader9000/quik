#pragma once

#include "reader.hpp"
#include <utils/filesystem/mapped_file.hpp>
#include <common/storage/serialize.hpp>

#include <zlib/zlib.h>
#include <memory>

namespace persistent {
namespace trade {
namespace quote {

static const unsigned place_for_header = common::storage::place_for_data<header>();


reader::reader(std::string const& path) {
    load_file_content(path);
}

reader::~reader() {}

void reader::load_file_content(std::string const& path) {
    utils::fs::mapped_file file(path.c_str());

    unsigned uncompressed_size_position = place_for_header;
    unsigned data_position              = place_for_header + sizeof(unsigned);

    auto header_ptr = reinterpret_cast<header*>(file.data());
    if ((header_ptr->flags_ & (1 << header::COMPRESSED)) == 0) {
        throw std::runtime_error("File must be compressed");
    }

    unsigned uncompressed_size = *reinterpret_cast<unsigned*>(place_for_header + file.data());
    
    auto data = reinterpret_cast<unsigned char*>(file.data() + data_position);

    bulk_ptr_ = std::make_shared<persistent::trade::quote::bulk>();
    bulk_ptr_->bulk_storage_.resize(uncompressed_size);
    bulk_ptr_->elements_count_ = header_ptr->quotes_count_;
    bulk_ptr_->sec_code_       = header_ptr->sec_code_;

    uLongf bulk_len = uncompressed_size;

    ::uncompress(bulk_ptr_->bulk_storage_.data(), &bulk_len, data,
                 common::numbers::integer_cast<uLong>(file.length() - data_position));
    assert(bulk_len == uncompressed_size);

}

void reader::read_all_quotes(quotes_sequence_t & buffer){
    assert(bulk_ptr_);
    bulk_ptr_->walk_throught([&](common::storage::quote& value) {
        buffer.push_back(value);
    });
}

} // namespace quote
} // namespace trade
} // namespace persistent