#pragma once

#include "Locked.hpp"

template<typename T>
class Protected {
    T val;
    mutable std::mutex mtx;
public:
    Locked<T&> lock() {
        return Locked<T&>{ val, std::unique_lock(mtx) };
    }
    Locked<const T&> lock() const {
        return Locked<const T&>{ val, std::unique_lock(mtx) };
    }
};
