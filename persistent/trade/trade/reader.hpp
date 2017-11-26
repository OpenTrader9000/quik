
#include "file.hpp"

#include "bulk.hpp"
#include <string>

namespace persistent {
namespace trade {
namespace trade {

struct reader {

    reader(std::string const& path);
    ~reader();

    void read_all_trades(trades_sequence_t& buffer);

    bulk_ptr_t const& bulk() { return bulk_ptr_; }

 private:
    void load_file_content(std::string const& path);

    bulk_ptr_t bulk_ptr_;
};
} // namespace quote
} // namespace trade
} // namespace persistent