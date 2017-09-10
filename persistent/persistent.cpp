
#include "persistent.hpp"
#include "impl/persistent.hpp"

namespace persistent {
    static std::unique_ptr< impl::persistent> instance_;


void init(::persistent::config const& config) {
    instance_ = std::make_unique<impl::persistent>(config);
    instance_->start();

}

void push_query(::common::message::ptr&& query_message) {
    assert(instance_);
    instance_->push_mv(std::move(query_message));
}

} // namespace persistent