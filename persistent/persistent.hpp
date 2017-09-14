#pragma once

#include "config.hpp"
#include <common/message/message.hpp>
#include <common/container/array_view.hpp>


namespace persistent {

    using message_t = common::message::ptr;
    template<typename T>
    using array_view_t = common::container::array_view<T>;
    using message_view_t = array_view_t<message_t>;

    void init(::persistent::config const& config);
    void push_event(message_t&& event_message);
    void push_query(message_t&& query_message);
    void push_queries(message_view_t messages);

    
    int create_scenario(char const* value);
    //void push_scenario_entry(::common::message::ptr&& scenario_entry_message);

    void stop();

    // TODO: create move ptr iterator
    // TODO: push_query(move_ptr_iterator it, size_t N); or array_view

    //void push_trade(::common::message::ptr&& query_message);
    //void push_quote(::common::message::ptr&& query_message);
}