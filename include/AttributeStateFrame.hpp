#pragma once

#include <unordered_map>

template<typename ATTR_GEN_T>
struct AttributeStateFrame {
    using attr_holder_t = typename ATTR_GEN_T::attr_holder_t;
    using attr_ptr_t = typename ATTR_GEN_T::attr_ptr_t;

    static AttributeStateFrame spawn(AttributeStateFrame& src) {
        AttributeStateFrame frame;
        frame.attr_ptr_map = src.attr_ptr_map;

        for(auto& [id, attr] : src.attr_holder_map) {
            frame.attr_ptr_map[id] = attr.get();
        }

        return frame;
    }
    static void merge(AttributeStateFrame& dst, AttributeStateFrame& src) {
        for(auto& [id, attr] : src.attr_holder_map) {
            if(auto it = dst.attr_holder_map.find(id); dst.attr_holder_map.end() != it)
                continue;

            if(auto it = dst.attr_ptr_map.find(id); dst.attr_ptr_map.end() != it) {
                dst.attr_ptr_map.erase(it);
            }

            dst.attr_holder_map[id] = std::move(attr);
        }
    }

    // holds attributes created or updated during the current frame
    std::unordered_map<size_t, attr_holder_t> attr_holder_map;
    std::unordered_map<size_t, attr_ptr_t> attr_ptr_map;

    attr_ptr_t get_attr(size_t id) {
        if(attr_holder_map.find(id) == attr_holder_map.end()) {
            // copy existing attribute if present
            if(auto it = attr_ptr_map.find(id); attr_ptr_map.end() != it) {
                attr_holder_map[id] = ATTR_GEN_T::gen_attribure(it->second);
                attr_ptr_map.erase(it);
                return attr_holder_map[id].get();
            }
            // create new attribute
            attr_holder_map[id] = ATTR_GEN_T::gen_attribure();
        }
        return attr_holder_map[id].get();
    }
    const attr_ptr_t get_attr(size_t id) const {
        if(auto it = attr_holder_map.find(id); attr_holder_map.end() != it) {
            return it->second.get();
        }
        if(auto it = attr_ptr_map.find(id); attr_ptr_map.end() != it) {
            return it->second;
        }
        return nullptr;
    }
};
