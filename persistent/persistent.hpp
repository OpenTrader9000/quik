#pragma once

#include "config.hpp"
#include <common/message/message.hpp>



namespace persistent {
    void init(::persistent::config const& config);
    void push_query(::common::message::ptr&& query_message);

    // TODO: create move ptr iterator
    // TODO: push_query(move_ptr_iterator it, size_t N); or array_view

    //void push_trade(::common::message::ptr&& query_message);
    //void push_quote(::common::message::ptr&& query_message);
}