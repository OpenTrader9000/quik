#pragma once

#include "traits.hpp"


namespace common {
namespace storage {
namespace compress {

template<typename Class, typename Type>
using member_ptr_t = Type Class::*;

template <typename Class, typename Type, typename Decoder, typename Encoder>
struct compressor_inst_t {
    Decoder decoder_;
    Encoder encoder_;

    member_ptr_t<Class, Type> pointer_to_member_;

    compressor_inst_t(member_ptr_t<Class, Type> ptr)
        : pointer_to_member_(ptr) {}

    // write to a field of the class
    void decode(Class& instance, unsigned char*& buffer, unsigned& size) {
        instance.*pointer_to_member_ = decoder_.decode<Type>(buffer, size);
    }

    // write into memory from field
    void encode(Class const& instance, unsigned char*& buffer, unsigned& size) {
        encoder_.compress(instance.*pointer_to_member_);
        encoder_.write(buffer, size);
    }
};

template<typename Class, typename Type>
auto compressor(member_ptr_t<Class, Type> member_pointer) {
    return compressor_inst_t<Class, Type, typename compress_traits<Type>::decoder_t, typename compress_traits<Type>::encoder_t>(
        member_pointer);
}

template <typename Class, typename Type>
auto history_compressor(member_ptr_t<Class, Type> member_pointer) {
    return compressor_inst_t<Class, Type, typename compress_traits<Type>::history_decoder_t, typename compress_traits<Type>::history_encoder_t>(
        member_pointer);
}


} // namespace compress
} // namespace storage
} // namespace common