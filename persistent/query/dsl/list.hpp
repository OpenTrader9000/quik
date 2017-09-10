#pragma once

#include <vector>

//namespace persistent {
//namespace query {
//namespace dsl {

#define DEFINE_DSL_LIST(FIELD_NAME, TYPE)            \
    struct FIELD_NAME##List {                        \
        std::vector<TYPE> list_;                     \
                                                     \
        FIELD_NAME##_List() {                        \
        }                                            \
        FIELD_NAME##_List(std::vector<TYPE>&& list)  \
        : list_(std::move(list)) {                   \
        }                                            \
                                                     \
        explicit operator std::vector<TYPE> const& { \
            return list_;                            \
        }                                            \
                                                     \
        explicit operator bool() const {             \
            return !list_.empty();                   \
        }                                            \
                                                     \
        template <typename T>                        \
        void operator()(T& query) {                  \
            query.FIELD_NAME = list_;                \
        }                                            \
    };

//     } // namespace dsl
//     } // namespace query
//     } // namespace persistent
