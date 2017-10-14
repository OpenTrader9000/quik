#pragma once

#include <iterator>
#include <utils/meta/apply.hpp>

namespace common {
namespace storage {

template <typename Serializable, typename Writer>
inline void visit_fields(Serializable& ser, Writer&& writer) {

    utils::apply_tuple(
    [&](auto field) mutable { writer(reinterpret_cast<char*>(&(ser.*field)), sizeof(ser.*field)); },
    Serializable::fields());
}

template<typename Serializable>
inline unsigned place_for_data() {
	unsigned size = 0;
	utils::apply_tuple([&](auto field) mutable {
		size += sizeof(((Serializable*)nullptr)->*field);
	}, Serializable::fields());
	return size;
}

} // namespace storage
} // namespace common