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
           std::enable_if_t<std::is_object_v<U>>* = nullptr)
        : val(std::move(v))
        , lk(std::move(l)) {}

    template <typename U = T>
    Locked(T v, std::unique_lock<std::mutex> l,
           std::enable_if_t<std::is_lvalue_reference_v<U>>* = nullptr)
        : val(v)
        , lk(std::move(l)) {}

    T& get() {
        return val;
    }
};
