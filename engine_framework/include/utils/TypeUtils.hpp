#pragma once

#include <type_traits>

template<typename T, typename ...Ts>
struct is_type_present {
    constexpr static bool val = (std::is_same_v<T, Ts> || ...);
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

template<typename ...Ts, typename ...Us>
struct types_intersect<types<Ts...>, types<Us...>> {
    constexpr static bool val = (is_type_present_v<Ts, Us...> || ...);
};

template<typename T, typename U>
constexpr bool types_intersect_v = types_intersect<T, U>::val;
