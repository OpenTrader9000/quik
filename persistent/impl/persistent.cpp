#pragma once

#include "persistent.hpp"
#include <common/message/event/flush.hpp>


namespace persistent {
namespace impl {

persistent::persistent(::persistent::config const& config)
: sql_(config.path2db_)
, trades_cache_(config.trades_archive_folder_)
, quotes_cache_(config.trades_archive_folder_)
{}

persistent::~persistent() {
}

void persistent::start(){
    sql_.start_sink();
    trades_cache_.start_sink();
	quotes_cache_.start_sink();
}

void persistent::stop() {
    sql_.push(common::message::make<common::message::event::flush>());
}


void persistent::push_mv(ptr_t&& message) {

    // flush send all. sql to sql
    auto code = message->code_;
    if (message.is_code(common::message::codes::FLUSH)) {
        // just sql for now
        sql_.push(message);
        trades_cache_.push(message);
		quotes_cache_.push(message);
    }
}

void persistent::push_cp(ptr_t const& message) {
    ptr_t tmp(message);
    push_mv(std::move(tmp));
}

} // namespace impl
} // namespace persistent