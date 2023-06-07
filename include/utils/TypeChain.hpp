#pragma once
#include <type_traits>

template<typename ...Ts>
struct is_type_present;

template<typename T, typename U>
struct is_type_present<T, U> {
    constexpr static bool val = std::is_same_v<T, U>;
};
template<typename T, typename U, typename ...Ts>
struct is_type_present<T, U, Ts...> {
    constexpr static bool val = std::is_same_v<T, U> | is_type_present<T, Ts...>::val;
};

template<typename ...Ts>
constexpr bool is_type_present_v = is_type_present<Ts...>::val;


template<typename ...Ts>
struct type_chain;

template<typename T>
struct type_chain<T> {
    using val_t = T;
    T val;
};

template<typename T, typename ... Ts>
struct type_chain<T, Ts...> {
    using val_t = std::enable_if_t<!is_type_present_v<T, Ts...>, T>;
    type_chain<Ts...> sub_chain;
    val_t val;
};

template<typename T, typename ...Ts>
T& get(type_chain<Ts...>& tc) {
    if constexpr(std::is_same_v<typename type_chain<Ts...>::val_t, T>) {
        return tc.val;
    } else {
        return get<T>(tc.sub_chain);
    }
}
