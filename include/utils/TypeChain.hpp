#pragma once
#include <type_traits>
#include <memory>

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


template<template<typename> typename TT, typename ...Ts>
struct tmpl_type_chain;

template<template<typename> typename TT, typename T>
struct tmpl_type_chain<TT, T> {
    using param_t = T;
    using val_t = TT<T>;
    val_t val;
};
template<template<typename> typename TT, typename T, typename ... Ts>
struct tmpl_type_chain<TT, T, Ts...> {
    using param_t = std::enable_if_t<!is_type_present_v<T, Ts...>, T>;
    using val_t = TT<param_t>;
    tmpl_type_chain<TT, Ts...> sub_chain;
    val_t val;
};

template<typename T, template<typename> typename TT, typename ...Ts>
TT<T>& get(tmpl_type_chain<TT, Ts...>& tc) {
    if constexpr(std::is_same_v<typename tmpl_type_chain<TT, Ts...>::param_t, T>) {
        return tc.val;
    } else {
        return get<T>(tc.sub_chain);
    }
}

template<template<typename> typename TT, typename ...Ts>
struct tmpl_type_chain_maker;

template<template<typename> typename TT, typename T>
struct tmpl_type_chain_maker<TT, T> {
    static tmpl_type_chain<TT, T> make(TT<T> val) {
        return {std::move(val)};
    }
};
template<template<typename> typename TT, typename T, typename ... Ts>
struct tmpl_type_chain_maker<TT, T, Ts...> {
    static tmpl_type_chain<TT, T, Ts...> make(TT<T> val, tmpl_type_chain<TT, Ts...> sub_chain) {
        return {std::move(sub_chain), std::move(val)};
    }
    static tmpl_type_chain<TT, T, Ts...> make(TT<T> val, TT<Ts>... vals) {
        auto sub_chain = tmpl_type_chain_maker<TT, Ts...>().make(std::move(vals)...);
        return tmpl_type_chain<TT, T, Ts...>(std::move(sub_chain),
                                             std::move(val));
    }
};

template<template<typename> typename TT, typename ... Ts>
tmpl_type_chain<TT, Ts...> make_tmpl_type_chain(TT<Ts> ... vals) {
    return tmpl_type_chain_maker<TT, Ts...>::make(std::move(vals)...);
}


