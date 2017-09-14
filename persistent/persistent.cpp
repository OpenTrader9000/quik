
#include "persistent.hpp"
#include "impl/persistent.hpp"

namespace persistent {
    static std::unique_ptr< impl::persistent> instance_;


void init(::persistent::config const& config) {
    instance_ = std::make_unique<impl::persistent>(config);
    instance_->start();
}

void push_event(message_t&& event_message) {
    // this function allowes push a message when instance of impl::persistent doesn't exists
    if (!instance_)
        return;
    instance_->push_mv(std::move(event_message));
}

void push_query(message_t&& query_message) {
    assert(instance_);
    instance_->push_mv(std::move(query_message));
}

void push_queries(message_view_t messages)
{
    using iter_t = decltype(messages.begin());

    assert(instance_);
    instance_->sql_.push_bulk(std::move_iterator<iter_t>(messages.begin()), messages.size());
}

int create_scenario(char const * value)
{
    assert(instance_);
    return instance_->sql_.create_scenario(value);
}

void stop()
{
    instance_->stop();
    instance_.reset();
}

} // namespace persistent