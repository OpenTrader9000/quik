#pragma once

#include <vector>

// namespace persistent {
// namespace query {
// namespace dsl {

#define DEFINE_DSL_LIST(FIELD_NAME)                                  \
template<typename type>                                              \
struct FIELD_NAME##_list_impl {                                      \
    type const& list_;                                               \
                                                                     \
    FIELD_NAME##_list_impl(type const& list)                         \
    : list_(list) {}                                                 \
                                                                     \
    operator type const&() const { return list_; }                   \
                                                                     \
    operator bool() const { return !list_.empty(); }                 \
                                                                     \
    template <typename T>                                            \
    void operator()(T& query) {                                      \
        query.FIELD_NAME.assign(list_.begin(), list_.end());         \
    }                                                                \
};                                                                   \
template<typename type>                                              \
inline FIELD_NAME##_list_impl<type>                                  \
    FIELD_NAME##_l(type&& list) {                                    \
    return FIELD_NAME##_list_impl<type>(std::forward<type>(list));   \
}
//     } // namespace dsl
//     } // namespace query
//     } // namespace persistent
