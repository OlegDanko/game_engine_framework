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
struct types {
    constexpr static bool none = false;
};


template<>
struct types<> {
    constexpr static bool none = true;
};


template<typename, typename>
struct types_intersect;

template<typename T, typename ...Us>
struct types_intersect<types<T>, types<Us...>> {
    constexpr static bool val = is_type_present_v<T, Us...>;
};

template<typename T, typename ...Ts, typename ...Us>
struct types_intersect<types<T, Ts...>, types<Us...>> {
    constexpr static bool val = is_type_present_v<T, Us...>
                                || types_intersect<types<Ts...>, types<Us...>>::val;
};

template<typename T, typename U>
constexpr bool types_intersect_v = types_intersect<T, U>::val;
