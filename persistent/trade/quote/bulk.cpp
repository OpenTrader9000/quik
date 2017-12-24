#include "bulk.hpp"

#include <common/numbers/cast.hpp>

namespace persistent {
namespace trade {
namespace quote {

bulk::bulk()
    : elements_count_(0){}

bulk::~bulk() {}

bulk_state bulk::walk(bulk_state state, std::function<bool(quote_t const&)> callback) const
{
    assert(state.position_in_buffer_ <= bulk_storage_.size());

    unsigned position = common::numbers::integer_cast<size_t>(state.position_in_buffer_);
    
    unsigned char const* begin = bulk_storage_.data() + position;
    unsigned char const* end   = bulk_storage_.data() + bulk_storage_.size();
    unsigned             size_to_end  = bulk_storage_.size() - position;

    auto compressor = common::storage::quote::make_compressor();
    common::storage::compress::set_compressor_state(compressor, state.quote_);
    
    quote_t value;

    while (begin != end) {
        common::storage::compress::decode(value, compressor, begin, size_to_end);
        if (!callback(value))
            break;
    }

    if (begin == end) {
        return bulk_state{ std::numeric_limits<unsigned>::max(), quote_t{} };
    }

    return bulk_state{ bulk_storage_.size() - static_cast<size_t>(size_to_end) , value };
}

} // namespace quote
} // namespace trade
} // namespace persistent