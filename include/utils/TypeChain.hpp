#pragma once
#include <type_traits>

template<typename ...Ts>
struct type_chain;


template<typename T>
struct type_chain<T> {
    using val_t = T;
    T val;
};

template<typename T, typename ... Ts>
struct type_chain<T, Ts...> {
    using val_t = T;
    type_chain<Ts...> sub_chain;
    T val;
};

template<typename T, typename ...Ts>
T& get(type_chain<Ts...>& tc) {
    if constexpr(std::is_same_v<typename type_chain<Ts...>::val_t, T>) {
        return tc.val;
    } else {
        return get<T>(tc.sub_chain);
    }
}
