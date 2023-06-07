#pragma once

#include "Locked.hpp"

template<typename T>
class Protected {
    T val;
    std::mutex mtx;
public:
    Locked<T&> lock() {
        return Locked<T&>{ val, std::unique_lock(mtx) };
    }
};
