#pragma once

#include "persistent.hpp"
#include <common/message/event/flush.hpp>


namespace persistent {
namespace impl {

persistent::persistent(::persistent::config const& config)
: sql_(config.path2db_)
// trade(config.trade_path_),
// quote(config.quote_path_)
{}

persistent::~persistent() {
}

void persistent::start(){
    sql_.start_sink();
}

void persistent::stop() {
    sql_.push(common::message::make<common::message::event::flush>());
}


void persistent::push_mv(ptr_t&& message) {

    // flush send all. sql to sql
    auto code = message->code_;
    // just sql for now
    sql_.push(std::move(message));
}

void persistent::push_cp(ptr_t const& message) {
    ptr_t tmp(message);
    push_mv(std::move(tmp));
}

} // namespace impl
} // namespace persistent