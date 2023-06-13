#pragma once
#include <memory>
#include "TypeUtils.hpp"


template<typename ...Ts>
struct type_chain;

template<>
struct type_chain<> {};

template<typename T>
struct type_chain<T> {
    using val_t = T;
    T val;
};

template<typename T, typename ... Ts>
struct type_chain<T, Ts...> {
    using val_t = std::enable_if_t<!is_type_present_v<T, Ts...>, T>;
    val_t val;
    type_chain<Ts...> sub_chain;
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

template<template<typename> typename TT>
struct tmpl_type_chain<TT> {};

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
    val_t val;
    tmpl_type_chain<TT, Ts...> sub_chain;
};

template<typename T, template<typename> typename TT, typename ...Ts>
TT<T>& get(tmpl_type_chain<TT, Ts...>& tc) {
    if constexpr(std::is_same_v<typename tmpl_type_chain<TT, Ts...>::param_t, T>) {
        return tc.val;
    } else {
        return get<T>(tc.sub_chain);
    }
}
template<typename T, template<typename> typename TT, typename ...Ts>
const TT<T>& get(const tmpl_type_chain<TT, Ts...>& tc) {
    if constexpr(std::is_same_v<typename tmpl_type_chain<TT, Ts...>::param_t, T>) {
        return tc.val;
    } else {
        return get<T>(tc.sub_chain);
    }
}

template<template<typename> typename TT, typename ... Ts>
struct tmpl_type_chain_maker {
    template<typename U>
    static tmpl_type_chain<TT, U>make_(TT<U>&& val) {
        return {std::forward<TT<U>>(val)};
    }
    template<typename U, typename ... Us>
    static tmpl_type_chain<TT, U, Us...> make_(TT<U>&& val, TT<Us>&&... vals) {
        return {std::forward<TT<U>>(val),
                make_<Us...>(std::forward<TT<Us>>(vals)...) };
    }
    static tmpl_type_chain<TT, Ts...> make(TT<Ts>&&... vals) {
        return make_<Ts...>(std::forward<TT<Ts>>(vals)...);
    }
};

template<template<typename> typename TT>
tmpl_type_chain<TT> make_tmpl_type_chain() {
    return {};
}

template<template<typename> typename TT, typename ... Ts>
tmpl_type_chain<TT, Ts...> make_tmpl_type_chain(TT<Ts>&& ... vals) {
    return tmpl_type_chain_maker<TT, Ts...>::make(std::forward<TT<Ts>>(vals)...);
}

template<typename CHAIN, typename T>
struct chain_contains;

template<typename T, typename ...Ts>
struct chain_contains<type_chain<Ts...>, T>{
    constexpr static bool val = is_type_present_v<T, Ts...>;
};
template<typename T, template<typename> typename TT, typename ...Ts>
struct chain_contains<tmpl_type_chain<TT, Ts...>, T>{
    constexpr static bool val = is_type_present_v<T, Ts...>;
};

template<typename CHAIN, typename T>
constexpr bool chain_contains_v = chain_contains<CHAIN, T>::val;

