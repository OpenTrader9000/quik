
#include "persistent.hpp"
#include "impl/persistent.hpp"

namespace persistent {
static std::unique_ptr<impl::persistent> instance_;


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

void push_queries(messages_view_t messages) {
    using iter_t = decltype(messages.begin());

    assert(instance_);
    instance_->sql_.push_bulk(std::move_iterator<iter_t>(messages.begin()), messages.size());
}

int create_scenario(char const* value) {
    assert(instance_);
    return instance_->sql_.create_scenario(value);
}

void stop() {
    instance_->stop();
    instance_.reset();
}

std::vector<scenario_t> extract_scenario_entries(std::string const&              scenario_name,
                                                 int                             idx,
                                                 unsigned                        count,
                                                 std::vector<std::string> const& callbacks,
                                                 uint64_t                        start_date_in_ms,
                                                 uint64_t                        end_date_in_ms) {
    assert(instance_);
    return instance_->sql_.extract_scenario_entries(scenario_name, idx, count, callbacks,
                                                    start_date_in_ms, end_date_in_ms);
}

int start_session(std::string name) {
    assert(instance_);
    return instance_->sql_.start_session(name);
}

void push_trade(message_t&& trade) {
    if (!instance_)
        return;
    instance_->trades_cache_.push(std::move(trade));
}

void push_trades(messages_view_t messages) {
    using iter_t = decltype(messages.begin());

    assert(instance_);
    instance_->trades_cache_.push_bulk(std::move_iterator<iter_t>(messages.begin()), messages.size());
}

void push_quote(message_t&& quote) {
    if (!instance_)
        return;
    instance_->quotes_cache_.push(std::move(quote));
}

void push_quotes(messages_view_t messages) {
    using iter_t = decltype(messages.begin());

    assert(instance_);
    instance_->quotes_cache_.push_bulk(std::move_iterator<iter_t>(messages.begin()), messages.size());
}

} // namespace persistent