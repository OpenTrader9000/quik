#pragma once

#define DEFINE_DSL_VALUE(VALUE_NAME)                                \
template<typename type>                                             \
struct VALUE_NAME##_value_impl {                                    \
        type const& value_;                                         \
                                                                    \
        VALUE_NAME##_value_impl(type const& val)                    \
        : value_(val) {}                                            \
                                                                    \
        template <typename T>                                       \
        void operator()(T& query) {                                 \
            query.VALUE_NAME = value_;                              \
        }                                                           \
};                                                                  \
template<typename T>                                                \
inline VALUE_NAME##_value_impl<T> VALUE_NAME##_v(T const& value) {  \
    return VALUE_NAME##_value_impl<T>(value);                       \
}

//template<typename type>                                             
//struct name_value_impl{ 
//        type const& value_;                                         
//                                                                    
//        name_value_impl(type const& val)
//        : value_(val) {}                                            
//                                                                    
//        template <typename T>                                       
//        void operator()(T& query) {                                 
//            query.VALUE_NAME = value_;                              
//        }                                                           
//};                                                                  
//template<typename T>                                                
//inline name_value_impl VALUE_NAME##_v(T const& value) {
//    return name_value_impl<T>(value);
//}
