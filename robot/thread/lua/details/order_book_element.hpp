#pragma once

#include <assert.h>
#include <common/numbers/bcd.hpp>

namespace robot {
namespace thread {
namespace lua {
namespace details {

// special class for an order book
// implements operators for this purposes
struct order_book_element {
    common::numbers::bcd price_;
    int                  quantity_; // quantity or diff

    // special operators for an element
    friend bool operator<(order_book_element const& lhs, order_book_element const& rhs) {
        return lhs.price_ < rhs.price_;
    }

    friend bool operator>(order_book_element const& lhs, order_book_element const& rhs) {
        return lhs.price_ > rhs.price_;
    }

    friend bool operator==(order_book_element const& lhs, order_book_element const& rhs) {
        return lhs.price_ == rhs.price_;
    }

    friend order_book_element operator-(order_book_element const& element, order_book_element const& diff) {
        assert(element.price_ == diff.price_);
        return order_book_element{ element.price_, element.quantity_ - diff.quantity_ };
    }

    friend order_book_element operator+(order_book_element const& element, order_book_element const& diff) {
        assert(element.price_ == diff.price_);
        return order_book_element{ element.price_, element.quantity_ + diff.quantity_ };
    }
};
} // namespace details
} // namespace lua
} // namespace thread
} // namespace robot