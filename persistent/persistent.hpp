#pragma once

#include "config.hpp"
#include "query/concrete/scenario_entry.hpp"
#include "pch.hpp"

namespace persistent {

    void init(::persistent::config const& config);
    void push_event(message_t&& event_message);
    void push_query(message_t&& query_message);
    void push_queries(messages_view_t messages);

    int create_scenario(char const* value);
    
    void stop();

    // extract queries
    std::vector<scenario_t> extract_scenario_entries(std::string const& scenario_name, int min_idx, unsigned max_count, array_view_t<std::string> callbacks);
    
    int start_session(std::string name = "default");

    //// pretty for running version
    //template<typename ...Args>
    //inline std::vector<scenario_t> extract_scenario_entries(Args&& ...args) {
    //    query::concrete::scenario_entry query_params(std::forward<Args>(args)...);
    //    return extract_scenario_entries(query_params.name, query_params.id, query_params.limit, query_params.callbacks);
    //}
    
    void push_trade(message_t&& trade);
    void push_trades(messages_view_t messages);

    //void push_quote(::common::message::ptr&& query_message);
}

// DSL part of the persistent
#include "query/dsl/range.hpp"
#include "query/dsl/list.hpp"
#include "query/dsl/value.hpp"


DEFINE_DSL_VALUE(name)     // name_v setup a value name
DEFINE_DSL_VALUE(limit)    // limit_v setup a value limit
DEFINE_DSL_VALUE(id)       // id_v setup a value id
DEFINE_DSL_LIST(callbacks) // callbacks_l