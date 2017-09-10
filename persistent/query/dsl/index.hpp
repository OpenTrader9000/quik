#pragma once


namespace persistent {
namespace query {
namespace dsl {

struct index {
    unsigned id_ = 0;
    
    index() {}
    index(unsigned id) : id_(id){}

    template<typename T>
    void operator()(T& query) {
        query.id_ = id_;
    }
};

struct index_range {
    unsigned low_ = 0;
    unsigned high_ = 0;

    index_range() {}
    index_range(unsigned low, unsigned high)
    : low_(low)
    , high_(high) {
    }

    template<typename T>
    void operator()(T& query) {
        query.index_range_ = *this;
    }
};



} // namespace dsl
} // namespace query
} // namespace persistent
