#pragma once

#include <unordered_map>

template <typename ATTR_GEN_T>
struct AttributeStateQueue {
    using attr_ptr_t = typename ATTR_GEN_T::attr_ptr_t;
    struct FrameReader {
        const attr_ptr_t read_attr(size_t id) const {}
    };
    struct FrameWriter {
        attr_ptr_t get_attr(size_t id) {}
        const attr_ptr_t read_attr(size_t id) const {}
    };
};
