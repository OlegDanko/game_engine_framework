#pragma once

#include <memory>

template<typename ATTR_T>
struct AttributeGeneratorDefault {
    using attr_holder_t = std::unique_ptr<ATTR_T>;
    using attr_ptr_t = ATTR_T*;
    static attr_holder_t gen_attribure() {
        return std::make_unique<ATTR_T>();
    }
    static attr_holder_t gen_attribure(attr_ptr_t attr) {
        auto attr_ret = std::make_unique<ATTR_T>();
        *attr_ret = *attr;
        return attr_ret;
    }
};

template<typename ATTR_GEN_T>
struct AttributeStateFrame;

template<typename ATTR_T>
using AttributeStateFrame_DefGen = AttributeStateFrame<AttributeGeneratorDefault<ATTR_T>>;
