#pragma once

#include <mutex>
#include <type_traits>

template<typename T>
class Locked {
    T val;
    std::unique_lock<std::mutex> lk;
public:
    template <typename U = T>
    Locked(T v, std::unique_lock<std::mutex> l,
           typename std::enable_if<std::is_object_v<U>>::type* = nullptr)
        : val(std::move(v))
        , lk(std::move(l)) {}

    template <typename U = T>
    Locked(T v, std::unique_lock<std::mutex> l,
           typename std::enable_if<std::is_lvalue_reference_v<U>>::type* = nullptr)
        : val(v)
        , lk(std::move(l)) {}

    T& get() {
        return val;
    }
};
