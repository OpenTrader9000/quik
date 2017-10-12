#pragma once

#include <common/message/message.hpp>
#include <common/container/array_view.hpp>
#include <common/storage/scenario_entry.hpp>


namespace persistent {

    using message_t = common::message::ptr;
    using scenario_t = common::storage::scenario_entry;

    template<typename T>
    using array_view_t = common::container::array_view<T>;
    using messages_view_t = array_view_t<message_t>;

}