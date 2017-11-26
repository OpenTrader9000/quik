#pragma once

#include "file.hpp"

#include <string>
#include "bulk.hpp"

namespace persistent {
namespace trade {
namespace quote {

struct reader {

    reader(std::string const& path);
    ~reader();

    void read_all_quotes(quotes_sequence_t& buffer);

    bulk_ptr_t const& bulk() const { return bulk_ptr_; }

 private:

     void load_file_content(std::string const& path);

     bulk_ptr_t    bulk_ptr_;
};
} // namespace quote
} // namespace trade
} // namespace persistent